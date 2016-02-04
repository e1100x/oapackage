# Orthogonal Array package
# pieter.eendebak@gmail.com

__all__ = ['oahelper']

import oalib
oalib.setloglevel(oalib.SYSTEM)
oalib.log_print(-oalib.SYSTEM, '')
from oalib import *
#import .oahelper
from . oahelper import *
#import .Doptim
from . Doptim import *
# import scanf
from . import scanf

import numpy as np

#%%

def autodoctest():
    """ Test the module using autodoc
    Example:
      >>> import oapackage
      >>> arrayclass=oapackage.arraydata_t(2, 40, 0, 7)
      >>> print(arrayclass)
      arrayclass: N 40, k 7, strength 0, s {2,2,2,2,2,2,2}, order 0

    """
    return


def unittest(verbose=1):
    """ Perform some unit testing, return True if succesfull """
    if verbose:
        print('oapackage: unittest: oalib version %s' % oalib.version())
    al = oalib.array_link()
    ii = 0
    al = oalib.exampleArray(ii, 0)
    
    if not isinstance(al.getarray(), np.ndarray):
        print('oapackage: unittest: error: array interface not working properly')
    else:
        if not al[2,0]==al.getarray()[2,0]:
            print('oapackage: unittest: error: array interface not working properly')
            
    arrayclass = oalib.arraylink2arraydata(al)
#    print(arrayclass)
    
    if verbose>=2:
        print('unittest: calculate efficiencies')
    Deff = al.Defficiency()
    aa=oalib.Aefficiencies(al)
    if verbose >= 2:
        print('## oapackage test: example array %d: Deff %.3f' % (ii, Deff))

    # DOP reduction
    if verbose>=2:
        print('unittest: test delete-one-factor GWLP reduction')
    al = oalib.exampleArray(5, 1)
    al2 = al.randomperm()
        
    alr=al.reduceDOP()    
    al2r=al2.reduceDOP()    
    if not alr==al2r:
        print('error: DOP reduced arrays unequal!: %d'  % (alr==al2r) )
        print('alr'); alr.showarraycompact()
        print('al2r'); al2r.showarraycompact()
        return False

    at = oalib.reductionDOP(al)
    check=at.apply(al)==al.reduceDOP()  
    if not check:
        print('error: DOP reduction transformation is invalid')

    # test graphtools
    if verbose>=2:
        print('unittest: test graphtools')
    #import . graphtools
    from . graphtools import oa2graph
    arrayclass = oalib.arraylink2arraydata(al)
    _ = oa2graph(al, arrayclass)
    return True

#%%
if __name__ == "__main__":
    """ Dummy main for oapackage """
    import doctest
    doctest.testmod()
