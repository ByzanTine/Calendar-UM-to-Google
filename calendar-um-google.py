# -*- coding: utf-8 -*-
#!/usr/bin/python
import HTMLParser  
import urlparse  
import urllib  
import urllib2  
import cookielib  
import string  
import re  
import sys
import getpass


usrname = raw_input("User-Name: ")
password = getpass.getpass('Password:')



#Home page
hosturl = 'https://weblogin.umich.edu/'

#post page
posturl = 'https://weblogin.umich.edu/cosign-bin/cosign.cgi'
#Cookie Handler
cj = cookielib.LWPCookieJar()  
cookie_support = urllib2.HTTPCookieProcessor(cj)  
opener = urllib2.build_opener(cookie_support, urllib2.HTTPHandler)  
urllib2.install_opener(opener)  
  
#Open the host page to obtain the cookie
h = urllib2.urlopen(hosturl)  
#print h.read()
#print cj

headers = {'User-Agent': 'Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36'

			}
postData = {'ref':'',
			'service':'',
			'required':'',
			'login':usrname,
			'password':password,
			'tokencode': ''
            }  
postData = urllib.urlencode(postData)  
request = urllib2.Request(posturl, postData, headers)  

response = urllib2.urlopen(request)
#print   response.code

text = response.read() 
# print '************************************************************************************'
# print text  
# check if authenticated
# print '************************************************************************************'
#print cj
#Redirect to Wolver
content= urllib2.urlopen('https://csprod.dsc.umich.edu/services/student')
#print content.geturl()
schedule= urllib2.urlopen('https://csprod.dsc.umich.edu/psc/csprodnonop/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_LIST.GBL')
#print schedule.read()
# Open a file
fo = open("output", "wb")
fo.write( schedule.read());

# Close opend file
fo.close()