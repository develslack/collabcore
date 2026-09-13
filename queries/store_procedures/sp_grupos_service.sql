DELIMITER //

CREATE PROCEDURE sp_insertar_grupo(
    IN p_nombre VARCHAR(100),
    IN p_id_creador INT
)
BEGIN
    INSERT INTO cc_grupos (nombre, id_creador)
    VALUES (p_nombre, p_id_creador);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END //

CREATE PROCEDURE sp_editar_grupo(
    IN p_id INT,
    IN p_nombre VARCHAR(100)
)
BEGIN
    UPDATE cc_grupos
    SET nombre = p_nombre
    WHERE id = p_id;
END //

DELIMITER ;
