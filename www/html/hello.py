#!/usr/bin/python3
# -*- coding: utf-8 -*-

import os

html_text = '<!DOCTYPE html>\n<html>\n<head>\n'
html_text += '\t<title>'+'test'+'</title>\n'
html_text += '\t<meta charset="utf-8">\n'
html_text += '</head>\n\n'
html_text += '<body>\n'

html_text += '<h3>Hello world by python cgi</h3>'

if os.getenv('QUERY_STRING') != None:
    html_text += 'String from browser: ' + os.getenv('QUERY_STRING')

html_text += '</body>\n</html>\n'

print (html_text)