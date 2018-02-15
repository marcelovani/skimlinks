Description
===========
This module transforms links to retailers into affiliate links, enabling you to
earn revenue when people click on the links and make purchases on retailers' 
websites.

Installation
============
Copy the module directory in to your Drupal /sites/all/modules directory
as per usual.


Configuration
=============
Go to /admin/config/services/skimlinks

Fill in the form with your Domain Id
The custom redirect subdomain should start with http/https
i.e. http://go.redirectingat.com

Choose if you want to use the Client side or Server side implementation.

Client side uses javascript to alter the links.

Server side alters the links during the execution. The main advantage of using
it is to make it work well on mobile phones
e.g. 
* Google Accelerated Mobile Pages (AMP)
* Facebook Instant Articles (IA)
* Apple News

In theory, you should only redirect to domains that are Skimlinks Merchants.
To find out if a domain is a Skimlink Merchant visit: 
http://developers.skimlinks.com/merchant.html. 

When you save a node, the links found in the fields will be checked using the 
Merchant API http://developers.skimlinks.com/merchant.html

All domains will be added/removed to/from the known domains automatically when
you save a node or when cron runs.

The blacklist is a global option that will avoid changing any link that points
to a domain in the list.
