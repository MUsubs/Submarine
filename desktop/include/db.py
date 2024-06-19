import sqlite3

import click
from flask import current_app, g
import os
def get_db():
    if 'db' not in g:
        g.db = sqlite3.connect(
            current_app.config['DATABASE'],
            detect_types=sqlite3.PARSE_DECLTYPES
        )
        g.db.row_factory = sqlite3.Row

    return g.db


def close_db(e=None):
    db = g.pop('db', None)

    if db is not None:
        db.close()

def init_db():
    print("Initializing Database")
    db = get_db()
    schema_path = os.path.normpath(os.path.join(os.path.dirname(os.path.dirname(current_app.instance_path)), r'include\schema.sql'))
    with current_app.open_resource(schema_path) as f:
        db.executescript(f.read().decode('utf8'))

@click.command('init-db')
def init_db_command():
    """Clear the existing data and create new tables."""
    init_db()
    click.echo('Initialized the database.')

def init_app(app, cli_cmd=False):
    print("Initializing app")
    app.teardown_appcontext(close_db)
    print("closed database")
    if cli_cmd:
        app.cli.add_command(init_db_command)
    else:
        init_db()
        