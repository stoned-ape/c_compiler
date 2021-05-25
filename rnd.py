import random as r

#this function generates random valid C expressions for testing purposes
def test(n,ops):
    if n==0: return "1"
    
    s=""
    for i in range(0,n):
        if r.randint(0,2)!=0: s+=str(r.randint(1,100))
        else: s+="("+test(n-1,ops)+")"
        idx=r.randint(0,len(ops)-1)
        s+=ops[idx]
    s+=str(r.randint(1,10))
    return s

ops=['+','-','*','%','/','&','|','^','!=','==','>','<','>=','<=','||','&&']

print("\nr="+test(5,['+','-','*','%','/','&','|','^'])+";\n")
