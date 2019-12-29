import urllib

getVars = {'var1': 'some_data', 'var2': 1337}
url = 'http://domain.com/somepage/?'

print(url + urllib.urlencode(getVars))