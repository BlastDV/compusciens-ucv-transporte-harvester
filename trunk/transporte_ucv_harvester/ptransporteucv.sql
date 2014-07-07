/*
SQLyog Community v11.5 (32 bit)
MySQL - 5.6.19-log : Database - ptransporteucv
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`ptransporteucv` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `ptransporteucv`;

/*Table structure for table `actividad` */

DROP TABLE IF EXISTS `actividad`;

CREATE TABLE `actividad` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `usuario` varchar(45) NOT NULL COMMENT 'El usuario actor',
  `tiempo` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT 'El momento del evento',
  `actividad` varchar(150) DEFAULT NULL COMMENT 'Descripcion del evento',
  PRIMARY KEY (`id`),
  KEY `fk_actividad_usuario_idx` (`usuario`),
  CONSTRAINT `fk_actividad_usuario` FOREIGN KEY (`usuario`) REFERENCES `usuario` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=88 DEFAULT CHARSET=utf8;

/*Data for the table `actividad` */

/*Table structure for table `parada` */

DROP TABLE IF EXISTS `parada`;

CREATE TABLE `parada` (
  `id` int(10) unsigned NOT NULL,
  `nombre` varchar(45) NOT NULL,
  `descripcion` varchar(255) DEFAULT NULL COMMENT 'Descripción de la parada: puntos de referencia del lugar, otros nombres, etc.',
  `ruta_id` int(10) unsigned NOT NULL COMMENT 'Ruta a la que pertenece',
  PRIMARY KEY (`id`),
  KEY `fk_parada_ruta_idx` (`ruta_id`),
  CONSTRAINT `fk_parada_ruta` FOREIGN KEY (`ruta_id`) REFERENCES `ruta` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `parada` */

/*Table structure for table `pasajero` */

DROP TABLE IF EXISTS `pasajero`;

CREATE TABLE `pasajero` (
  `cedula` int(8) unsigned NOT NULL COMMENT 'La cedula de los pasajeros',
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `pasajero` */

/*Table structure for table `registro` */

DROP TABLE IF EXISTS `registro`;

CREATE TABLE `registro` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `viaje_id` bigint(20) unsigned NOT NULL COMMENT 'El viaje asociado al registro',
  `pasajero_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `fk_registro_viaje_idx` (`viaje_id`),
  KEY `fk_registro_pasajero_idx` (`pasajero_id`),
  CONSTRAINT `fk_registro_viaje` FOREIGN KEY (`viaje_id`) REFERENCES `viaje` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_registro_pasajero` FOREIGN KEY (`pasajero_id`) REFERENCES `pasajero` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `registro` */

/*Table structure for table `ruta` */

DROP TABLE IF EXISTS `ruta`;

CREATE TABLE `ruta` (
  `origen` varchar(45) NOT NULL COMMENT 'El origen de la ruta',
  `destino` varchar(45) NOT NULL COMMENT 'El final de la ruta',
  `id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `ruta` */

/*Table structure for table `transportista` */

DROP TABLE IF EXISTS `transportista`;

CREATE TABLE `transportista` (
  `primer_nombre` varchar(45) NOT NULL,
  `segundo_nombre` varchar(45) DEFAULT NULL COMMENT 'El segundo nombre es opcional.',
  `primer_apellido` varchar(45) NOT NULL,
  `segundo_apellido` varchar(45) NOT NULL,
  `fecha_nac` date DEFAULT NULL,
  `cedula` int(8) unsigned NOT NULL,
  PRIMARY KEY (`cedula`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `transportista` */

/*Table structure for table `usuario` */

DROP TABLE IF EXISTS `usuario`;

CREATE TABLE `usuario` (
  `id` varchar(15) NOT NULL COMMENT 'El id del usuario',
  `password` varchar(41) NOT NULL COMMENT 'La clave encriptada',
  `permisos` varchar(20) NOT NULL COMMENT 'El id de los permisos correspondientes',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `usuario` */

insert  into `usuario`(`id`,`password`,`permisos`) values ('admin','d033e22ae348aeb5660fc2140aec35850c4da997','RW'),('josue','8cb2237d0679ca88db6464eac60da96345513964','NN'),('krys','8cb2237d0679ca88db6464eac60da96345513964','RN');

/*Table structure for table `viaje` */

DROP TABLE IF EXISTS `viaje`;

CREATE TABLE `viaje` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `fecha` date NOT NULL COMMENT 'La fecha del viaje',
  `hora_salida` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT 'Hora de partida',
  `hora_llegada` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'Hora de llegada',
  `ruta_id` int(10) unsigned NOT NULL COMMENT 'La ruta del viaje',
  `ci_transportista` int(8) unsigned NOT NULL COMMENT 'La cedula del chofer',
  PRIMARY KEY (`id`),
  KEY `fk_viaje_transportista` (`ci_transportista`),
  KEY `fk_viaje_ruta` (`ruta_id`),
  CONSTRAINT `fk_viaje_transportista` FOREIGN KEY (`ci_transportista`) REFERENCES `transportista` (`cedula`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_viaje_ruta` FOREIGN KEY (`ruta_id`) REFERENCES `ruta` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `viaje` */

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
