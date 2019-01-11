CREATE TABLE cultivations (
    id          INTEGER     PRIMARY KEY AUTOINCREMENT,
    name        TEXT
);

CREATE TABLE types (
    id          INTEGER     PRIMARY KEY AUTOINCREMENT,
    name        TEXT
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

INSERT INTO types(name) VALUES ("temp");
INSERT INTO types(name) VALUES ("airHumid");
INSERT INTO types(name) VALUES ("soilHumid");

INSERT INTO cultivations(name) VALUES ("The one cultivation to rule them all");
