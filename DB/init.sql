
-- ROLES
CREATE TABLE IF NOT EXISTS Roles (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nombre TEXT NOT NULL UNIQUE,
    descripcion TEXT
);

-- USUARIOS
CREATE TABLE IF NOT EXISTS Usuarios (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nombre TEXT NOT NULL,
    email TEXT NOT NULL UNIQUE,
    contrasena TEXT NOT NULL,
    id_rol INTEGER NOT NULL,
    media_puntuacion REAL DEFAULT 0.0,
    total_puntos INTEGER DEFAULT 0,
    fecha_registro TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (id_rol) REFERENCES Roles(id)
);

-- RETOS
CREATE TABLE IF NOT EXISTS Retos (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    titulo TEXT NOT NULL,
    descripcion TEXT,
    estado TEXT DEFAULT 'SIN_COMENZAR' CHECK (estado IN ('SIN_COMENZAR', 'EN_CURSO', 'FINALIZADO')),
    dificultad TEXT DEFAULT 'FACIL' CHECK (dificultad IN ('FACIL', 'MEDIO', 'DIFICIL')),
    tipo TEXT DEFAULT 'CTF' CHECK (tipo IN ('CTF', 'HACKATHON')),
    limite_plazas INTEGER NOT NULL,
    fecha_inicio TEXT NOT NULL,
    fecha_fin TEXT NOT NULL,
    fecha_inicio_inscripcion TEXT NOT NULL,
    fecha_fin_inscripcion TEXT NOT NULL,
    puntos INTEGER DEFAULT 100,
    id_organizador INTEGER NOT NULL,
    fecha_creacion TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (id_organizador) REFERENCES Usuarios(id)
);

-- EQUIPOS
CREATE TABLE IF NOT EXISTS Equipos (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nombre TEXT NOT NULL,
    id_reto INTEGER NOT NULL,
    num_miembros INTEGER DEFAULT 0,
    ids_usuarios TEXT,
    fecha_creacion TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (id_reto) REFERENCES Retos(id),
    UNIQUE(id_reto, nombre)
);

-- PARTICIPACIONES
CREATE TABLE IF NOT EXISTS Participaciones (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    id_usuario INTEGER NOT NULL,
    id_reto INTEGER NOT NULL,
    id_equipo INTEGER,
    motivacion TEXT,
    puntos INTEGER DEFAULT 0,
    estado TEXT DEFAULT 'EN_ESPERA' CHECK (estado IN ('EN_ESPERA', 'ACEPTADO', 'RECHAZADO')),
    fecha_solicitud TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (id_usuario) REFERENCES Usuarios(id),
    FOREIGN KEY (id_reto) REFERENCES Retos(id),
    FOREIGN KEY (id_equipo) REFERENCES Equipos(id),
    UNIQUE(id_usuario, id_reto)
);

INSERT INTO Roles (id, nombre, descripcion) VALUES
(1, 'ORGANIZADOR', 'Usuario que organiza retos'),
(2, 'HACKER', 'Usuario que participa en retos'),
(3, 'STAFF', 'Usuario que ayuda en la organizacion');

INSERT INTO Usuarios (id, nombre, email, contrasena, id_rol, media_puntuacion, total_puntos) VALUES
(1, 'Juan García', 'juan@example.com', 'pass123', 1, 0, 0),
(2, 'María López', 'maria@example.com', 'pass456', 2, 85.5, 1250),
(3, 'Carlos Pérez', 'carlos@example.com', 'pass789', 2, 92.0, 1850),
(4, 'Ana Martínez', 'ana@example.com', 'pass000', 2, 78.5, 950),
(5, 'Pedro Ruiz', 'pedro@example.com', 'pass111', 2, 88.0, 1450);

INSERT INTO Retos (id, titulo, descripcion, estado, dificultad, tipo, limite_plazas, fecha_inicio, fecha_fin, fecha_inicio_inscripcion, fecha_fin_inscripcion, puntos, id_organizador) VALUES
(1, 'CTF Principiantes', 'Capture the flag para principiantes', 'EN_CURSO', 'FACIL', 'CTF', 20, '2026-04-01', '2026-04-15', '2026-03-25', '2026-04-01', 100, 1),
(2, 'Hackathon Web', 'Desarrollo de aplicación web completa', 'SIN_COMENZAR', 'MEDIO', 'HACKATHON', 10, '2026-04-20', '2026-04-22', '2026-04-10', '2026-04-20', 200, 1),
(3, 'CTF Avanzado', 'Capture the flag nivel avanzado', 'FINALIZADO', 'DIFICIL', 'CTF', 15, '2026-03-01', '2026-03-15', '2026-02-15', '2026-03-01', 300, 1);

INSERT INTO Equipos (id, nombre, id_reto, num_miembros, ids_usuarios) VALUES
(1, 'Team Alpha', 1, 3, '2,3,4'),
(2, 'Team Beta', 1, 2, '5'),
(3, 'Team Gamma', 2, 3, '2,3,5'),
(4, 'Team Delta', 2, 2, '4'),
(5, 'Team Epsilon', 3, 4, '2,3,4,5');

INSERT INTO Participaciones (id, id_usuario, id_reto, id_equipo, motivacion, puntos, estado) VALUES
(1, 2, 1, 1, 'Quiero mejorar mis habilidades en seguridad', 85, 'ACEPTADO'),
(2, 3, 1, 1, 'Me interesa aprender sobre CTF', 90, 'ACEPTADO'),
(3, 4, 1, 1, 'Experiencia en el campo', 78, 'ACEPTADO'),
(4, 5, 1, 2, 'Primera participación en CTF', 65, 'ACEPTADO'),
(5, 2, 2, 3, 'Desarrollo web es mi pasión', 0, 'EN_ESPERA'),
(6, 3, 2, 3, 'Ampliar experiencia en hackathon', 0, 'EN_ESPERA'),
(7, 5, 2, 3, 'Aprender nuevas tecnologías', 0, 'EN_ESPERA'),
(8, 4, 2, 4, 'Interesada en participar', 0, 'EN_ESPERA'),
(9, 2, 3, 5, 'Reto de nivel superior', 250, 'ACEPTADO'),
(10, 3, 3, 5, 'Desafío muy interesante', 280, 'ACEPTADO'),
(11, 4, 3, 5, 'Experiencia previa en CTF', 240, 'ACEPTADO'),
(12, 5, 3, 5, 'Nivel avanzado en seguridad', 270, 'ACEPTADO');