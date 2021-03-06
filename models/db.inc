<?php
include_once ('models/Permission.inc');
include_once ('models/user.inc');
include_once ('models/Group.inc');
include_once ('include/Logger.inc');

//Class code taken from ToDoUpload example by Scott Sharkey cs.franklin.edu/~sharkesc

class Db {
    public static function getDb() {
        try {
   $dbname = ###############################
            $host = ########################################
            $username= ##################################
            $password = ######################
            // see if we need to create tables
            
        
            $db = new PDO("pgsql:host=$host; port=5432;dbname=$dbname;user=$username;password=$password");
            

            // force exceptions for better debugging.
            $db -> setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            
            // force cascading deletes on foreign key constraints
            
        //    $query = $db ->query("SELECT * FROM user");
        //    if ($query == false){
         //       $makeDb = true;
           // }
        
            if ($makeDb) {
                self::makeTables($db);
                self::populateTables($db);
            }

        }  
          catch (PDOException $e) {
            die("Could not open database. " . $e -> getMessage());
        }
        return $db;
    }

    private static function makeTables(&$db) {
        
        Logger::instance() -> info("Creating tables");
        // @formatter:off
        $statements = array( 
            "CREATE TABLE IF NOT EXISTS  users
            (
              id serial PRIMARY KEY,
              email text NOT NULL,
              first_name text NOT NULL,
              last_name text NOT NULL,
              password text NOT NULL
              
            )",
                "CREATE TABLE IF NOT EXISTS groups (
                id serial PRIMARY KEY,
                name TEXT NOT NULL
            )",
            "CREATE TABLE IF NOT EXISTS permissions (
                id serial PRIMARY KEY,
                name TEXT NOT NULL
            )",
            "CREATE TABLE IF NOT EXISTS usergroups (
                userId INTEGER NOT NULL,
                groupId INTEGER NOT NULL,
                FOREIGN KEY(userId) REFERENCES users(id) ON DELETE CASCADE,
                FOREIGN KEY(groupId) REFERENCES groups(id) ON DELETE CASCADE
            )",
            "CREATE TABLE IF NOT EXISTS grouppermissions (
                permissionId INTEGER NOT NULL,
                groupId INTEGER NOT NULL,
                FOREIGN KEY(permissionId) REFERENCES permissions(id) ON DELETE CASCADE,
                FOREIGN KEY(groupId) REFERENCES groups(id) ON DELETE CASCADE
            )",
            "CREATE TABLE IF NOT EXISTS QUESTIONS(
            id serial PRIMARY KEY,       
            post_date DATE NOT NULL,       
            title CHAR NOT NULL,       
            post CHAR NOT NULL,
            tags CHAR,       
            user_id INTEGER,       
            FOREIGN KEY(user_id) REFERENCES USERS(id) ON DELETE CASCADE       
            );",
            "CREATE TABLE IF NOT EXISTS ANSWERS(
               id INTEGER PRIMARY KEY NOT NULL,        
               post_date DATE  NOT NULL,       
               post VARCHAR NOT NULL,        
               user_id INTEGER NOT NULL,        
               post_id INTEGER NOT NULL,     
               FOREIGN KEY(user_id) REFERENCES USERS(id) ON DELETE CASCADE,       
               FOREIGN KEY (post_id) REFERENCES QUESTIONS(id) ON DELETE CASCADE
           
);",

                "CREATE TABLE IF NOT EXISTS file (
                id serial PRIMARY KEY,
                dir VARCHAR(150) NOT NULL,
                origName VARCHAR(50) NOT NULL,
                nameOnDisk VARCHAR(50) NOT NULL,
                type VARCHAR(50) NOT NULL,
                size INTEGER NOT NULL,
                questionId INTEGER NOT NULL,
                FOREIGN KEY(questionId) REFERENCES QUESTIONS(id) ON DELETE CASCADE
            )"
        );
        // @formatter:on

        $db -> beginTransaction();
        foreach ($statements as $statement) {
            $st = $db -> prepare($statement);
            $st -> execute();
        }
        $db -> commit();
    }

    private static function populateTables(&$db) {
        // create the permissions
        $userperms = array('answer_create', 'question_create');
        $adminperms = array('admin_page', 'edit_user', 'delete_user');
        $editorperms = array('answer_delete', 'question_delete', 'question_edit');
        $permissions = array();
        $allperms = array($adminperms, $userperms, $editorperms);
        foreach ($allperms as $perms) {
            foreach ($perms as $name) {
                $p = new Permission( array('name' => $name));
                $p -> insert();
                $permissions[$name] = $p;
            }
        }

        // set up Users group for new registrants
        Logger::instance() -> info("Creating default Users group.");
        $g = new Group( array('name' => 'Users'));
        $g -> insert();
        $u = new User( array('first_name' => 'Damien', 'last_name' => 'Pyles', 'email' => 'rikari@insight.rr.com', 'password' =>    'JambaJuice!'));
        $u->insert();
        $g->addUser($u);
        foreach ($userperms as $name) {
            $g -> addPermission($permissions[$name]);
        }
        
            Logger::instance() -> info("Creating default Editor group.");
        $ge = new Group( array('name' => 'Editor'));
        $ge -> insert();
        foreach ($editorperms as $name) {
            $ge -> addPermission($permissions[$name]);
        }
        $u = new User( array('first_name' => 'System', 'last_name' => 'Editor', 'email' => 'editor@example.com', 'password' => 'Asdf12!'));
        $u -> insert();
        $g->addUser($u);
        $ge->addUser($u);
        
        

        // set up the super-user
        Logger::instance() -> info("Creating default Administrators group.");
        $u = new User( array('first_name' => 'System', 'last_name' => 'Administrator', 'email' => 'admin@example.com', 'password' => 'asdF123$'));
        $u -> insert();
        $ga = new Group( array('name' => 'Administrators'));
        $ga -> insert();
        $g -> addUser($u);
        $ga-> addUser($u);
        $ge->addUser($u);
        foreach ($adminperms as $name) {
            $ga -> addPermission($permissions[$name]);
        }

    }
}
?>
