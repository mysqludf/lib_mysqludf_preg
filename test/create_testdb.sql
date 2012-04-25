
Use mysql;


######################################################################
# Taken from libmysql_udf_xql
###################################################################

--disable_warnings
DROP DATABASE IF EXISTS `preg_test`;
--enable_warnings

CREATE DATABASE `preg_test`;
USE `preg_test`;

CREATE TABLE `state` (
  `code` varchar(2) NOT NULL,
  `country_code` varchar(2) NOT NULL,
  `description` varchar(255) NOT NULL,
  PRIMARY KEY  (`code`)
) ENGINE=HEAP DEFAULT CHARSET=latin1;

INSERT INTO `state` VALUES ('al','us','Alabama'),('ak','us','Alaska'),('as','us','American Samoa'),('az','us','Arizona'),('ar','us','Arkansas'),('ca','us','California'),('co','us','Colorado'),('ct','us','Connecticut'),('de','us','Delaware'),('dc','us','District of Columbia'),('fm','us','Federated States of Micronesia'),('fl','us','Florida'),('ga','us','Georgia'),('gu','us','Guam'),('hi','us','Hawaii'),('id','us','Idaho'),('il','us','Illinois'),('in','us','Indiana'),('ia','us','Iowa'),('ks','us','Kansas'),('ky','us','Kentucky'),('la','us','Louisiana'),('me','us','Maine'),('mh','us','Marshall Islands'),('md','us','Maryland'),('ma','us','Massachusetts'),('mi','us','Michigan'),('mn','us','Minnesota'),('ms','us','Mississippi'),('mo','us','Missouri'),('mt','us','Montana'),('ne','us','Nebraska'),('nv','us','Nevada'),('nh','us','New Hampshire'),('nj','us','New Jersey'),('nm','us','New Mexico'),('ny','us','New York'),('nc','us','North Carolina'),('nd','us','North Dakota'),('mp','us','Northern Mariana Islands'),('oh','us','Ohio'),('ok','us','Oklahoma'),('or','us','Oregon'),('pw','us','Palau'),('pa','us','Pennsylvania'),('pr','us','Puerto Rico'),('ri','us','Rhode Island'),('sc','us','South Carolina'),('sd','us','South Dakota'),('tn','us','Tennessee'),('tx','us','Texas'),('ut','us','Utah'),('vt','us','Vermont'),('vi','us','Virgin Island'),('va','us','Virginia'),('wa','us','Washington'),('wv','us','West Virginia'),('wi','us','Wisconsin'),('wy','us','Wyoming'),('ab','ca','Alberta'),('bc','ca','British Columbia'),('mb','ca','Manitoba'),('nb','ca','New Brunswick'),('nf','ca','New Foundland'),('nt','ca','Northwest Territories'),('ns','ca','Nova Scotia'),('on','ca','Ontario'),('pe','ca','Prince Edward Island'),('pq','ca','Quebec'),('sk','ca','Saskatchewan'),('yt','ca','Yukon Territories');

