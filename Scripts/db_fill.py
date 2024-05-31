# Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.

# Copyright (c) 2023, Technology Innovation Institute, Yas Island, Abu Dhabi,
# United Arab Emirates.

# Purpose:
# --------------------- Script to Fill in Shares Table with Dummy Content
import sys
import mysql.connector

# Read command-line arguments
if len(sys.argv) < 5:
    print("Please write down the following <host> <user> <password> <DB_name>")
    sys.exit(1)

host = sys.argv[1]
user = sys.argv[2]
password = sys.argv[3]
database = sys.argv[4]

# Establish a connection to the MySQL database
conn = mysql.connector.connect(
    host=host,
    user=user,
    password=password,
    database=database
)

# Create a cursor object
cursor = conn.cursor()
players = 3
amount = 10000
# Loop to generate and execute the insert statements
for i in range(players):
    for _ in range(amount):
        player = i
        channel = 777778  # use any custom channel
        share = 1
        mac_share = ''

        sql = "INSERT INTO `share` (player, channel, share, mac_share) VALUES (%s, %s, %s, %s)"
        val = (player, channel, share, mac_share)

        cursor.execute(sql, val)

# Commit the transaction
conn.commit()

# Close the cursor and connection
cursor.close()
conn.close()

print(f"{amount} entries have been inserted into the share table.")
