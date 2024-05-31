CREATE SCHEMA IF NOT EXISTS $MYSQL_DATABASE;

CREATE TABLE IF NOT EXISTS authenticated_material
(
    ID                      INTEGER       NOT NULL AUTO_INCREMENT,
    PLAYER                  INTEGER       NOT NULL,
    MATERIAL_TYPE           INTEGER       NOT NULL,
    MATERIAL                LONGTEXT      NOT NULL,
    CHOICEBITS              VARCHAR(256)  NOT NULL,
    USED                    bool          NOT NULL DEFAULT false,
    CREATION_DATE           TIMESTAMP     NOT NULL DEFAULT NOW(),
    CONSTRAINT material_id
        PRIMARY KEY (ID)
);

CREATE TABLE IF NOT EXISTS garbled_circuit
(
    ID                       INTEGER       NOT NULL AUTO_INCREMENT,
    PLAYER                   INTEGER       NOT NULL,
    CIRCUIT_NUMBER           INTEGER       NOT NULL,
    CIRCUIT_CONTENT          LONGTEXT      NOT NULL,
    CHOICEBITS               VARCHAR(256)  NOT NULL,
    USED                     bool          NOT NULL DEFAULT false,
    CREATION_DATE            TIMESTAMP     NOT NULL DEFAULT NOW(),
    CONSTRAINT circuit_id
        PRIMARY KEY (ID)
) ENGINE = InnoDB;