from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import subprocess
from ansi2html import Ansi2HTMLConverter
import cgi
import re

class JSONHandler(BaseHTTPRequestHandler):
    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'POST')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()

    def do_POST(self):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()

        content_length = int(self.headers['Content-Length'])
        form = cgi.FieldStorage(
            fp=self.rfile,
            headers=self.headers,
            environ={'REQUEST_METHOD': 'POST',
                     'CONTENT_TYPE': self.headers['Content-Type']}
        )
        if(form['file'].filename is not None):
            with open( './engine/' + form['file'].filename, 'wb') as f:
                f.write(form['file'].value)

        query = form['command'].value.split(';')[0]
        command = 'cd ./engine; ./' + query
        prog = query.split(' ')[0]

        # "./ld <filename>"     [loads file]
        # "./ld <filename> <some-string>"    [loads file and calculates diameter]
        # "./rd"   [reads global data about the graph]
        # "./ret  <node-id>"   [reads all data about the node]
        # "./ret <node-id> <type-of-data>"  [outputs only the desired type of data]
        # "./q_scc <node-id-1> <node-id-2>"   [binary output, whether they belong on the same SCC]
        # "./s_path <node-id-1> <node-id-2>"  [shortest path]
        # "./neigh <node-id-1>"  [prints outgoing neighbours]
        if(prog not in ['ld', 'rd', 'ret', 'ret', 'ret', 'q_scc', 's_path', 'neigh']):
            restext = 'Invalid Command'
        else:
            try:
                output = subprocess.check_output(command, shell=True, stderr=subprocess.STDOUT)
                restext = str(output.decode('utf-8'))
            except subprocess.CalledProcessError as e:
                print('Command failed with exit code', e.returncode, 'and output:')
                restext = str(e.output.decode('utf-8'))


        # do something with the data
        # print(restext)
        restext = restext.replace('<', '&lt')
        restext = restext.replace('>', '&gt')
        result = {'status': 'success', 'output': restext}
        self.wfile.write(json.dumps(result).encode('utf-8'))

def run_server():
    address = ('', 8000)
    httpd = HTTPServer(address, JSONHandler)
    print('Server listening on port 8000...')
    httpd.serve_forever()

if __name__ == '__main__':
    run_server()

