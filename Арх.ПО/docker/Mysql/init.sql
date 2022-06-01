CREATE DATABASE IF NOT EXISTS stud;

USE stud;

DROP TABLE IF EXISTS Person;

CREATE TABLE Person (
  login varchar(255) NOT NULL PRIMARY KEY,
  first_name VARCHAR(255) NOT NULL,
  last_name VARCHAR(255) NOT NULL,
  age INT NOT NULL,
  CHECK (age>=0),
  KEY fn (first_name),
  KEY ln (last_name)
);




INSERT INTO Person (login, first_name, last_name, age) VALUES
('boba', 'Boba', 'Bibovich', FLOOR(RAND()*(100-1)+1)),
('biba2007thebest', 'Biba', 'Bobovsky', FLOOR(RAND()*(100-1)+1));

