CREATE TABLE cultivations (
    id          INTEGER     PRIMARY KEY AUTOINCREMENT,
    name        TEXT
);

CREATE TABLE types (
    id          INTEGER     PRIMARY KEY AUTOINCREMENT,
    name        TEXT,
    interval    INTEGER
);

CREATE TABLE stats (
    id          INTEGER     PRIMARY KEY AUTOINCREMENT,
    value       REAL,
    type        INTEGER,
    cultivation INTEGER,
    date        TEXT,

    FOREIGN KEY(type) REFERENCES types(id),
    FOREIGN KEY(cultivation) REFERENCES cultivations(id)
);

INSERT INTO types(id, name, interval) VALUES (0, "growth", 60);
INSERT INTO types(id, name, interval) VALUES (1, "airTemp", 60);
INSERT INTO types(id, name, interval) VALUES (2, "airHumid", 60);
INSERT INTO types(id, name, interval) VALUES (3, "soilHumid", 60);

INSERT INTO cultivations(name) VALUES ("The one cultivation to rule them all");
