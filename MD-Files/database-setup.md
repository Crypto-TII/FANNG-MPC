# Database set up
## With docker compose
There is docker-compose to launch a MySQL server in the root directory.
You will need to have docker client already installed in your computer to run it.
Launch it running the following command from the root folder of the project:

```
docker-compose up --build -d
```

Docker will set up the databases with the MySQL provided in your `.env` file, that should be placed in the root folder of the project as well.  
A [.env.sample](../.env.sample) file has been created as an example.
You can simply duplicate and rename the file to `.env` and change the values you consider.

```
#.env.sample content
MYSQL_ROOT_USER=root
MYSQL_ROOT_PASSWORD=<your-password>
MYSQL_PORT=3306
```

To create the schemas and tables, there is a script too [here](../database/setup-mpclib-database.sh).
It will create 9 databases, all of them with the tables needed for both the Input/Output database system and the Offline Garbling feature.
```sh ./database/setup-mpclib-database.sh```

This script needs the following environment variables to be set, with the same values you provided in your .env file:
```
export MYSQL_HOST_URL="127.0.0.1"
export MYSQL_PORT=3306
export MYSQL_ROOT_USER="root"
export MYSQL_ROOT_PASSWORD="<your-password>"
```

Once the command has finished running, the following schemas will have been created in your MySQL server:
- mpclib-database-test → _testing purpose database_
- mpclib-database-0 →  _production database_
- mpclib-database-1 → _production database_
- mpclib-database-2 → _production database_
- mpclib-database-3 → _production database_
- mpclib-database-4 → _production database_
- mpclib-database-dev-0 → _development database_
- mpclib-database-dev-1 → _development database_
- mpclib-database-dev-2 → _development database_
- mpclib-database-dev-3 → _development database_
- mpclib-database-dev-4 → _development database_

## Without docker-compose
Also, you could create the databases on your own (or use an existing one) and just create the tables with:

- [io schema](../database/schema-io.sql)
- [gc schema](/database/schema-gc.sql)

## Testing database
A single database could be set up for running the database dependent tests.
This means that in testing environments such as Jenkins all the database config files for each player would have to contain the same test database parameters. In other words, all the database config files will have the same content.
