#! /usr/bin/env python

"""code_generator.py

Usage:
    code_generator.py FILE... (--output=<header>)

Options:
    -h, --help                      show help
    FILE                            json file to split or join and name of output (last)
    -o=<header>, --output=<header>  name of output file header C++

Output is include/butcher_methods.hpp
"""

import json
import os, sys
import sympy as sp

sys.path.append("../analysis")
from analysis import rk_butcher

def is_explicit(filename):
  with open(filename,'r') as f:
    data = json.load(f)

  rk = rk_butcher(
    label=data['label'],
    A=data['A'],b=data['b'],c=data['c']
  )
  return rk.is_explicit

def extract_meth(filename):
  with open(filename,'r') as f:
    data = json.load(f)

  id = os.path.basename(filename).replace(".json","")
  print(id)

  rk = rk_butcher(
    label=data['label'],
    A=data['A'],b=data['b'],c=data['c'],
    b2=data['b2'] if 'b2' in data else None
  )
  
  A = rk.A.evalf(n=36).tolist()
  b = rk.b.evalf(n=36).T.tolist()[0]
  c = rk.c.evalf(n=36).T.tolist()[0]

  butcher = {
    'label': data['label'],
    'id': id,
    'A':[ " , ".join(map(str,ai)) for ai in A ],
    'b':" , ".join(map(str,b)),
    'c':" , ".join(map(str,c)),
    'order': rk.order
  }

  if 'b2' in data:
    b2 = sp.Matrix(data['b2']).evalf(n=36).T.tolist()[0]
    butcher['b2'] = " , ".join(map(str,b2))
  
  return butcher

  
  
if __name__ == '__main__':
    from docopt import docopt

    args = docopt( __doc__ , sys.argv[1:] )

    list_meth = [ extract_meth(f) for f in args['FILE'] if is_explicit(f) ]

    local_dir = os.path.dirname(os.path.abspath(__file__))

    import jinja2
    env = jinja2.Environment(loader=jinja2.FileSystemLoader(local_dir))
    template = env.get_template("tpl_butcher_methods.hxx")

    with open(args['--output'],'w') as f:
      f.write(
        template.render(list_meth=list_meth)
      )
