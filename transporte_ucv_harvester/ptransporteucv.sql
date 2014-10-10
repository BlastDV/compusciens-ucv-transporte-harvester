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
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

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

insert  into `parada`(`id`,`nombre`,`descripcion`,`ruta_id`) values (10001,'Casalta II','Bloque 5, Casalta II',2019),(10002,'Propatria','Estación de Metro Propatria',2019),(10003,'Av. Sucre','Estacion de Metro Gato Negro',2019),(10004,'Av. Urdaneta','Centro Comercial Galerías Ávila',2019),(10005,'UCV_CENTRAL','UCV Sede principal',2019),(20001,'El Junquito','Km. 4, Coco Frío',1047),(20002,'El Amparo','Modulo Policial El Amparo',1047),(20003,'Plaza Catia','Plaza Catia',1047),(30001,'UCV_CENTRAL','UCV Sede principal',1045),(30002,'Petare','Estacion de Metro Petare',1045),(30003,'Guarenas','Guarenas',1045),(30004,'Guatire','Guatire',1045),(40001,'UCV_CENTRAL','UCV Sede principal',2022);

/*Table structure for table `pasajero` */

DROP TABLE IF EXISTS `pasajero`;

CREATE TABLE `pasajero` (
  `cedula` int(8) unsigned NOT NULL COMMENT 'La cedula de los pasajeros',
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=252 DEFAULT CHARSET=utf8;

/*Data for the table `pasajero` */

insert  into `pasajero`(`cedula`,`id`) values (17489960,156),(14199311,157),(20858077,158),(21447855,159),(14999633,160),(19874225,161),(22458996,162),(17484757,163),(14763123,164),(23410705,165),(21000101,166),(14685741,167),(17655101,168),(19484747,169),(17635141,170),(19675140,171),(11688105,172),(11788839,173),(21789931,174),(23701171,175),(16458961,176),(14555123,177),(10553143,178),(22546332,179),(22753312,180),(12053922,181),(24101778,182),(20111763,183),(19031753,184),(20871763,185),(14277858,186),(21536559,187),(17489960,188),(14199311,189),(20858077,190),(21447855,191),(14999633,192),(19874225,193),(22458996,194),(17484757,195),(14763123,196),(23410705,197),(21000101,198),(14685741,199),(17655101,200),(19484747,201),(17635141,202),(19675140,203),(11788839,204),(21789931,205),(23701171,206),(16458961,207),(14555123,208),(10553143,209),(22546332,210),(22753312,211),(12053922,212),(24101778,213),(14199311,214),(20111763,215),(19031753,216),(20871763,217),(14277858,218),(21536559,219),(17489960,220),(20858077,221),(21447855,222),(14999633,223),(19874225,224),(22458996,225),(17484757,226),(14763123,227),(23410705,228),(21000101,229),(14685741,230),(17655101,231),(19484747,232),(17635141,233),(19675140,234),(11688105,235),(11788839,236),(21789931,237),(23701171,238),(16458961,239),(14555123,240),(10553143,241),(22546332,242),(22753312,243),(12053922,244),(24101778,245),(20111763,246),(19031753,247),(20871763,248),(14277858,249),(14555123,250),(22753312,251);

/*Table structure for table `registro` */

DROP TABLE IF EXISTS `registro`;

CREATE TABLE `registro` (
  `viaje_id` bigint(20) unsigned NOT NULL COMMENT 'El viaje asociado al registro',
  `pasajero_id` int(10) unsigned NOT NULL COMMENT 'Un pasajero asociado al viaje',
  PRIMARY KEY (`viaje_id`,`pasajero_id`),
  KEY `fk_registro_viaje_idx` (`viaje_id`),
  KEY `fk_registro_pasajero_idx` (`pasajero_id`),
  CONSTRAINT `fk_registro_pasajero` FOREIGN KEY (`pasajero_id`) REFERENCES `pasajero` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_registro_viaje` FOREIGN KEY (`viaje_id`) REFERENCES `viaje` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `registro` */

insert  into `registro`(`viaje_id`,`pasajero_id`) values (7,156),(7,157),(7,158),(7,159),(7,160),(7,161),(7,162),(7,163),(7,164),(7,165),(7,166),(7,167),(7,168),(7,169),(7,170),(7,171),(7,172),(7,173),(7,174),(7,175),(7,176),(7,177),(7,178),(7,179),(7,180),(7,181),(7,182),(7,183),(7,184),(7,185),(7,186),(8,187),(8,188),(8,189),(8,190),(8,191),(8,192),(8,193),(8,194),(8,195),(8,196),(8,197),(8,198),(8,199),(8,200),(8,201),(8,202),(8,203),(9,204),(9,205),(9,206),(9,207),(9,208),(9,209),(9,210),(9,211),(9,212),(9,213),(9,214),(9,215),(9,216),(9,217),(9,218),(10,219),(10,220),(11,221),(11,222),(11,223),(11,224),(11,225),(11,226),(11,227),(11,228),(11,229),(11,230),(11,231),(11,232),(11,233),(11,234),(11,235),(11,236),(11,237),(11,238),(11,239),(11,240),(11,241),(11,242),(11,243),(11,244),(11,245),(12,246),(12,247),(12,248),(12,249),(12,250),(12,251);

/*Table structure for table `ruta` */

DROP TABLE IF EXISTS `ruta`;

CREATE TABLE `ruta` (
  `origen` varchar(45) NOT NULL COMMENT 'El origen de la ruta',
  `destino` varchar(45) NOT NULL COMMENT 'El final de la ruta',
  `id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `ruta` */

insert  into `ruta`(`origen`,`destino`,`id`) values ('UCV_CENTRAL','Guatire',1045),('UCV_CENTRAL','Guarenas',1046),('El Junquito','UCV_CENTRAL',1047),('UCV_CENTRAL','Los Teques',1050),('UCV_CENTRAL','La Guaira',1053),('Petare','UCV_CENTRAL',1061),('UCV_Veterinaria','UCV_CENTRAL',1123),('Casalta II','UCV_CENTRAL',2019),('UCV_CENTRAL','El Junquito',2022);

/*Table structure for table `transportista` */

DROP TABLE IF EXISTS `transportista`;

CREATE TABLE `transportista` (
  `primer_nombre` varchar(45) NOT NULL,
  `segundo_nombre` varchar(45) DEFAULT NULL COMMENT 'El segundo nombre es opcional.',
  `primer_apellido` varchar(45) NOT NULL,
  `segundo_apellido` varchar(45) NOT NULL,
  `fecha_nac` date DEFAULT NULL,
  `cedula` int(8) unsigned NOT NULL,
  `activo` tinyint(1) NOT NULL DEFAULT '1' COMMENT 'El transportista esta activo?',
  PRIMARY KEY (`cedula`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `transportista` */

insert  into `transportista`(`primer_nombre`,`segundo_nombre`,`primer_apellido`,`segundo_apellido`,`fecha_nac`,`cedula`,`activo`) values ('Antonio','José','Rivas','Beltrand','1989-09-09',11688105,1),('Raul','Ernesto','Pereira','Figueroa','1970-10-03',12394033,1),('Ernesto','Saul','Camacho','Montilla','1971-07-07',14199311,1),('Ricardo','Antonio','Pereira','Mora','1976-12-07',14396554,1),('Alberto','José','Maneiro','Fuentes','1980-04-30',16475014,0),('Ruben','Yaimeru','Maza','Marquez','1993-02-10',20875996,1),('Pedro','Alejandro','Fernandes','Olarte','1993-06-29',21536559,0);

/*Table structure for table `usuario` */

DROP TABLE IF EXISTS `usuario`;

CREATE TABLE `usuario` (
  `id` varchar(15) NOT NULL COMMENT 'El id del usuario',
  `password` varchar(41) NOT NULL COMMENT 'La clave encriptada',
  `permisos` varchar(60) NOT NULL COMMENT 'El id de los permisos correspondientes',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `usuario` */

insert  into `usuario`(`id`,`password`,`permisos`) values ('admin','d033e22ae348aeb5660fc2140aec35850c4da997','R.W/R.C.M.D/R.C.M.S.D'),('andre','eebf0871c30b9dbb4560b2ed9c5987bc2c8a8ef6','R/R.C/R.M.D/'),('josue','8cb2237d0679ca88db6464eac60da96345513964','R/R.D/R/'),('krys','8cb2237d0679ca88db6464eac60da96345513964','R/R.D/R.S'),('rubenyai','0d552caec67cd2ccfbd4344c2b72e6a35ee982b8','-/R/.S/');

/*Table structure for table `viaje` */

DROP TABLE IF EXISTS `viaje`;

CREATE TABLE `viaje` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `fecha` date NOT NULL COMMENT 'La fecha del viaje',
  `hora_salida` time NOT NULL COMMENT 'Hora de partida',
  `hora_llegada` time DEFAULT NULL COMMENT 'Hora de llegada',
  `ruta_id` int(10) unsigned NOT NULL COMMENT 'La ruta del viaje',
  `ci_transportista` int(8) unsigned NOT NULL COMMENT 'La cedula del chofer',
  PRIMARY KEY (`id`),
  KEY `fk_viaje_transportista` (`ci_transportista`),
  KEY `fk_viaje_ruta` (`ruta_id`),
  CONSTRAINT `fk_viaje_ruta` FOREIGN KEY (`ruta_id`) REFERENCES `ruta` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_viaje_transportista` FOREIGN KEY (`ci_transportista`) REFERENCES `transportista` (`cedula`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;

/*Data for the table `viaje` */

insert  into `viaje`(`id`,`fecha`,`hora_salida`,`hora_llegada`,`ruta_id`,`ci_transportista`) values (7,'2014-10-10','14:07:00','16:44:48',2019,21536559),(8,'2014-10-10','00:00:00','16:47:36',2022,11688105),(9,'2014-10-10','00:00:00','16:47:36',2022,11688105),(10,'2014-10-10','00:00:00','16:49:49',2022,14199311),(11,'2014-10-10','00:00:00','16:49:49',2022,14199311),(12,'2014-10-10','00:00:00','16:49:49',2022,14199311);

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
