-- Create the database used by the MySQL repository.
CREATE DATABASE IF NOT EXISTS library_app
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_0900_ai_ci;

USE library_app;

-- One row in this table represents one library::Book.
CREATE TABLE IF NOT EXISTS books (
    id INT NOT NULL PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    author VARCHAR(255) NOT NULL,
    publication_year INT NOT NULL,
    borrowed BOOLEAN NOT NULL DEFAULT FALSE
);
