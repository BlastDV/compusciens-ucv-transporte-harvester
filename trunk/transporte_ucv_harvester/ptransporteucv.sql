/*
SQLyog Community v11.42 (32 bit)
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

/*Table structure for table `paradas` */

DROP TABLE IF EXISTS `paradas`;

CREATE TABLE `paradas` (
  `nombre_parada` varchar(45) NOT NULL,
  `id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `paradas` */

/*Table structure for table `pasajeros` */

DROP TABLE IF EXISTS `pasajeros`;

CREATE TABLE `pasajeros` (
  `cedula` int(8) unsigned NOT NULL COMMENT 'La cedula de los pasajeros',
  `id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `pasajeros` */

/*Table structure for table `registros` */

DROP TABLE IF EXISTS `registros`;

CREATE TABLE `registros` (
  `viaje_id` bigint(20) unsigned NOT NULL COMMENT 'El viaje asociado al registro',
  `pasajero_id` int(10) unsigned NOT NULL COMMENT 'La lista de pasajeros asociada al viaje',
  `id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `Un registro, un viaje` (`viaje_id`),
  CONSTRAINT `Un registro, un viaje` FOREIGN KEY (`viaje_id`) REFERENCES `viajes` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `registros` */

/*Table structure for table `rutas` */

DROP TABLE IF EXISTS `rutas`;

CREATE TABLE `rutas` (
  `origen` varchar(45) NOT NULL COMMENT 'El origen de la ruta',
  `destino` varchar(45) NOT NULL COMMENT 'El final de la ruta',
  `paradas_id` int(10) unsigned NOT NULL COMMENT 'Referencia las paradas de la ruta',
  `id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `Muchas paradas, una ruta` (`paradas_id`),
  CONSTRAINT `Muchas paradas, una ruta` FOREIGN KEY (`paradas_id`) REFERENCES `paradas` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `rutas` */

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

/*Table structure for table `usuarios` */

DROP TABLE IF EXISTS `usuarios`;

CREATE TABLE `usuarios` (
  `password` varchar(41) NOT NULL COMMENT 'La clave encriptada',
  `id` varchar(15) NOT NULL COMMENT 'El id del usuario',
  `permisos` int(10) unsigned DEFAULT NULL COMMENT 'El id de los permisos correspondientes',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `usuarios` */

insert  into `usuarios`(`password`,`id`,`permisos`) values ('d033e22ae348aeb5660fc2140aec35850c4da997','admin',NULL);

/*Table structure for table `viajes` */

DROP TABLE IF EXISTS `viajes`;

CREATE TABLE `viajes` (
  `fecha` date NOT NULL COMMENT 'La fecha del viaje',
  `hora_salida` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT 'Hora de partida',
  `hora_llegada` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT 'Hora de llegada',
  `ruta_id` int(10) unsigned NOT NULL COMMENT 'La ruta del viaje',
  `ci_transportista` int(8) unsigned NOT NULL COMMENT 'La cedula del chofer',
  `id` bigint(20) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `Un viaje, un transportista` (`ci_transportista`),
  KEY `Un viaje, una ruta` (`ruta_id`),
  CONSTRAINT `Un viaje, un transportista` FOREIGN KEY (`ci_transportista`) REFERENCES `transportista` (`cedula`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `Un viaje, una ruta` FOREIGN KEY (`ruta_id`) REFERENCES `rutas` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `viajes` */

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
