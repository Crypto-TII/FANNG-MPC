CREATE SCHEMA IF NOT EXISTS $MYSQL_DATABASE;

CREATE TABLE IF NOT EXISTS share
(
    ID               INTEGER       NOT NULL AUTO_INCREMENT,
    PLAYER           INTEGER       NOT NULL,
    CHANNEL          INTEGER       NOT NULL,
    SHARE            VARCHAR(200)  NOT NULL,
    MAC_SHARE        VARCHAR(200)  NULL,
    CREATION_DATE    TIMESTAMP     NOT NULL DEFAULT NOW(),
    CONSTRAINT share_id
        PRIMARY KEY (ID)
);

CREATE TABLE IF NOT EXISTS cint
(
    ID               INTEGER        NOT NULL AUTO_INCREMENT,
    PLAYER           INTEGER        NOT NULL,
    CHANNEL          INTEGER        NOT NULL,
    VALUE            VARCHAR(200)   NOT NULL,
    CREATION_DATE    TIMESTAMP      NOT NULL DEFAULT NOW(),
    CONSTRAINT integer_id
        PRIMARY KEY (ID)
);

CREATE TABLE IF NOT EXISTS regint
(
    ID               INTEGER        NOT NULL AUTO_INCREMENT,
    PLAYER           INTEGER        NOT NULL,
    CHANNEL          INTEGER        NOT NULL,
    VALUE            VARCHAR(200)   NOT NULL,
    CREATION_DATE    TIMESTAMP      NOT NULL DEFAULT NOW(),
    CONSTRAINT integer_id
        PRIMARY KEY (ID)
);

CREATE TABLE IF NOT EXISTS gfp
(
    ID               INTEGER        NOT NULL AUTO_INCREMENT,
    PLAYER           INTEGER        NOT NULL,
    CHANNEL          INTEGER        NOT NULL,
    VALUE            VARCHAR(200)   NOT NULL,
    CREATION_DATE    TIMESTAMP      NOT NULL DEFAULT NOW(),
    CONSTRAINT integer_id
        PRIMARY KEY (ID)
);

DELIMITER //
DROP PROCEDURE IF EXISTS InsertBatchData;
CREATE PROCEDURE InsertBatchData(IN batch_size INT, IN channel_id INT, IN player_id INT, IN input_string MEDIUMTEXT)
BEGIN
    DECLARE total_records INT;
    DECLARE records_processed INT DEFAULT 0;
    DECLARE current_record MEDIUMTEXT;
    -- Get the total number of records in the input string
    SET total_records = batch_size;
    -- Start the transaction
    START TRANSACTION;
    WHILE records_processed < total_records DO
        -- Extract each record from the input string
        SET current_record = SUBSTRING_INDEX(input_string, ';', 1);
        SET input_string = SUBSTRING(input_string, LENGTH(current_record) + 2); -- 1 for the semicolon

        -- Extract the rest of the values from the current record
        SET @share_value = SUBSTRING_INDEX(current_record, ' ', 1);
        IF INSTR(current_record, ' ') > 0 THEN
        	SET @mac_share_value = SUBSTRING_INDEX(current_record, ' ', -1);
        ELSE 
	        SET @mac_share_value = '';
        END IF;

        -- Insert the record into the table
        INSERT INTO share (player, channel, share, mac_share)
        VALUES (player_id, channel_id, @share_value, @mac_share_value);
        SET records_processed = records_processed+1;
    END WHILE;
    -- Commit any remaining records
    COMMIT;
END //
DELIMITER ;