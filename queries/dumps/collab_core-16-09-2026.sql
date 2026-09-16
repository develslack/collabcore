/*M!999999\- enable the sandbox mode */ 
-- MariaDB dump 10.19  Distrib 10.5.29-MariaDB, for Linux (x86_64)
--
-- Host: localhost    Database: collab_core
-- ------------------------------------------------------
-- Server version	10.5.29-MariaDB

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `cc_acl_documentos`
--

DROP TABLE IF EXISTS `cc_acl_documentos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cc_acl_documentos` (
  `id_documento` int(11) NOT NULL,
  `id_usuario` int(11) NOT NULL,
  `permiso_octal` tinyint(3) unsigned NOT NULL,
  `otorgado_por` int(11) NOT NULL,
  `fecha_asignacion` datetime DEFAULT current_timestamp(),
  PRIMARY KEY (`id_documento`,`id_usuario`),
  KEY `id_usuario` (`id_usuario`),
  KEY `otorgado_por` (`otorgado_por`),
  CONSTRAINT `cc_acl_documentos_ibfk_1` FOREIGN KEY (`id_documento`) REFERENCES `cc_documentos` (`id`),
  CONSTRAINT `cc_acl_documentos_ibfk_2` FOREIGN KEY (`id_usuario`) REFERENCES `cc_usuarios` (`id`),
  CONSTRAINT `cc_acl_documentos_ibfk_3` FOREIGN KEY (`otorgado_por`) REFERENCES `cc_usuarios` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cc_acl_documentos`
--

LOCK TABLES `cc_acl_documentos` WRITE;
/*!40000 ALTER TABLE `cc_acl_documentos` DISABLE KEYS */;
/*!40000 ALTER TABLE `cc_acl_documentos` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cc_documentos`
--

DROP TABLE IF EXISTS `cc_documentos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cc_documentos` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `id_propietario` int(11) NOT NULL,
  `id_grupo_padre` int(11) DEFAULT NULL,
  `titulo` varchar(255) NOT NULL,
  `tipo` enum('texto','planilla') NOT NULL,
  `estado_ciclo` varchar(50) DEFAULT 'abierto',
  `fecha_creacion` datetime DEFAULT current_timestamp(),
  PRIMARY KEY (`id`),
  KEY `id_propietario` (`id_propietario`),
  KEY `id_grupo_padre` (`id_grupo_padre`),
  CONSTRAINT `cc_documentos_ibfk_1` FOREIGN KEY (`id_propietario`) REFERENCES `cc_usuarios` (`id`),
  CONSTRAINT `cc_documentos_ibfk_2` FOREIGN KEY (`id_grupo_padre`) REFERENCES `cc_grupos` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cc_documentos`
--

LOCK TABLES `cc_documentos` WRITE;
/*!40000 ALTER TABLE `cc_documentos` DISABLE KEYS */;
/*!40000 ALTER TABLE `cc_documentos` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cc_grupos`
--

DROP TABLE IF EXISTS `cc_grupos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cc_grupos` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `nombre` varchar(100) NOT NULL,
  `nombre_directorio` varchar(100) DEFAULT NULL,
  `id_creador` int(11) NOT NULL,
  `fecha_creacion` datetime DEFAULT current_timestamp(),
  PRIMARY KEY (`id`),
  KEY `id_creador` (`id_creador`),
  CONSTRAINT `cc_grupos_ibfk_1` FOREIGN KEY (`id_creador`) REFERENCES `cc_usuarios` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cc_grupos`
--

LOCK TABLES `cc_grupos` WRITE;
/*!40000 ALTER TABLE `cc_grupos` DISABLE KEYS */;
INSERT INTO `cc_grupos` VALUES (5,'Grupo A','grupo_5',1,'2026-09-15 13:16:47');
/*!40000 ALTER TABLE `cc_grupos` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cc_miembros_grupo`
--

DROP TABLE IF EXISTS `cc_miembros_grupo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cc_miembros_grupo` (
  `id_grupo` int(11) NOT NULL,
  `id_usuario` int(11) NOT NULL,
  `es_coordinador` tinyint(1) DEFAULT 0,
  `fecha_ingreso` datetime NOT NULL DEFAULT current_timestamp(),
  `fecha_baja` datetime DEFAULT NULL,
  PRIMARY KEY (`id_grupo`,`id_usuario`,`fecha_ingreso`),
  KEY `id_usuario` (`id_usuario`),
  CONSTRAINT `cc_miembros_grupo_ibfk_1` FOREIGN KEY (`id_grupo`) REFERENCES `cc_grupos` (`id`),
  CONSTRAINT `cc_miembros_grupo_ibfk_2` FOREIGN KEY (`id_usuario`) REFERENCES `cc_usuarios` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cc_miembros_grupo`
--

LOCK TABLES `cc_miembros_grupo` WRITE;
/*!40000 ALTER TABLE `cc_miembros_grupo` DISABLE KEYS */;
/*!40000 ALTER TABLE `cc_miembros_grupo` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cc_roles`
--

DROP TABLE IF EXISTS `cc_roles`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cc_roles` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `descripcion` varchar(100) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cc_roles`
--

LOCK TABLES `cc_roles` WRITE;
/*!40000 ALTER TABLE `cc_roles` DISABLE KEYS */;
INSERT INTO `cc_roles` VALUES (1,'Administrador'),(2,'Usuario');
/*!40000 ALTER TABLE `cc_roles` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cc_usuarios`
--

DROP TABLE IF EXISTS `cc_usuarios`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cc_usuarios` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `nombre` varchar(50) NOT NULL,
  `email` varchar(100) NOT NULL,
  `password` varchar(260) NOT NULL,
  `rol_id` int(11) NOT NULL,
  `fecha_creacion` datetime DEFAULT current_timestamp(),
  PRIMARY KEY (`id`),
  UNIQUE KEY `email` (`email`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cc_usuarios`
--

LOCK TABLES `cc_usuarios` WRITE;
/*!40000 ALTER TABLE `cc_usuarios` DISABLE KEYS */;
INSERT INTO `cc_usuarios` VALUES (1,'Administrador','root@gmail.com','ffd49b5c1f27ac18caa45079e3de14f282491395',1,'2026-09-12 12:29:18'),(2,'Augusto Maza','debianmaza@gmail.com','313e7c6732075a99760972caeaa147eb7bbdd64c',2,'2026-09-14 12:05:44');
/*!40000 ALTER TABLE `cc_usuarios` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Dumping routines for database 'collab_core'
--
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_baja_miembro` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_baja_miembro`(
    IN p_id_grupo INT,
    IN p_id_usuario INT
)
BEGIN
    
    
    
    UPDATE cc_miembros_grupo
    SET fecha_baja = CURRENT_TIMESTAMP
    WHERE id_grupo = p_id_grupo
      AND id_usuario = p_id_usuario
      AND fecha_baja IS NULL;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_editar_grupo` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_editar_grupo`(
    IN p_id INT,
    IN p_nombre VARCHAR(100)
)
BEGIN
    
    UPDATE cc_grupos
    SET nombre = p_nombre
    WHERE id = p_id;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_editar_rol` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_editar_rol`(
    IN p_id INT,
    IN p_descripcion VARCHAR(128)
)
BEGIN
    UPDATE cc_roles
    SET descripcion = p_descripcion
    WHERE id = p_id;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_insertar_grupo` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_insertar_grupo`(
    IN p_nombre VARCHAR(100),
    IN p_id_creador INT
)
BEGIN
    DECLARE v_nuevo_id INT;
    DECLARE v_nombre_dir VARCHAR(100);

    
    INSERT INTO cc_grupos (nombre, id_creador)
    VALUES (p_nombre, p_id_creador);

    
    SET v_nuevo_id = LAST_INSERT_ID();

    
    SET v_nombre_dir = CONCAT('grupo_', v_nuevo_id);

    
    UPDATE cc_grupos
    SET nombre_directorio = v_nombre_dir
    WHERE id = v_nuevo_id;

    
    SELECT v_nuevo_id AS nuevo_id, v_nombre_dir AS nombre_directorio;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_insertar_miembro` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_insertar_miembro`(
    IN p_id_grupo INT,
    IN p_id_usuario INT,
    IN p_es_coordinador TINYINT
)
BEGIN
    
    
    
    INSERT INTO cc_miembros_grupo (id_grupo, id_usuario, es_coordinador, fecha_ingreso)
    VALUES (p_id_grupo, p_id_usuario, p_es_coordinador, CURRENT_TIMESTAMP);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_insertar_rol` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_insertar_rol`(
    IN p_descripcion VARCHAR(128)
)
BEGIN
    INSERT INTO cc_roles (descripcion)
    VALUES (p_descripcion);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_obtener_todos_roles` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_obtener_todos_roles`()
BEGIN
    SELECT id, descripcion FROM cc_roles;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-09-16 14:24:44
