import sqlite3
import os
from db import get_db, close_db, init_db, init_db_command, init_app
# Path to the SQLite database file
current_cwd = os.getcwd()
print(current_cwd)
db_relativepath = r'\desktop\include\flaskr.sqlite'
db_path2 = current_cwd + db_relativepath
print(f"Path2 = {db_path2}")
db_path = os.path.normpath(db_path2)
print(f"Path2 = {db_path2}")
# Check if the database file exists
if not os.path.exists(db_path):
    print(f"Database file '{db_path}' not found.")
else:
    try:
        # Connect to the SQLite database
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()

        # Check if the 'temperature' table exists
        cursor.execute("SELECT * FROM temperature")
        table_exists = cursor.fetchall()
        print(table_exists)
        # Close the connection
        conn.close()
    except sqlite3.Error as e:
        print(f"An error occurred: {e}")
