DROP TABLE IF EXISTS temperature;
DROP TABLE IF EXISTS target_destinations;

CREATE TABLE temperature (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  temperature_value FLOAT NOT NULL
);

CREATE TABLE target_destinations(
    id integer PRIMARY KEY AUTOINCREMENT,
    x FLOAT NOT NULL,
    y FLOAT NOT NULL,
    z FLOAT NOT NULL
)