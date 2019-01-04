
import sys
tokens = []
stack = ["$","A"]
####################################################LEXICAL ANALYSER##############################################################
import re
file = open(sys.argv[1], "r")
fileinput = file.readlines()
lexemelist = []
class lexeme:
    type = ''
    value = ''
    def __init__(self, type, value):
        self.type = type
        self.value = value
commentcounter = 0
legalkeywords = ["else","if","int","return","void","while","float"]
def commentstripper(mystr):
    blocked = False
    newstring = ''
    global commentcounter
    for i in range(0,len(mystr)):
        if blocked:
            blocked = False
        else:
            if mystr[i] == "/":
                if i < len(mystr) - 1:
                    if mystr[i + 1] == "*":
                        blocked = True
                        commentcounter += 1
                    elif commentcounter < 1:
                        newstring += mystr[i]
            elif commentcounter > 0:
                if mystr[i] == "*":
                    if i < len(mystr) - 1:
                        if mystr[i + 1] == "/":
                            blocked = True
                            commentcounter += -1
            elif commentcounter < 1:
                newstring += mystr[i]
    return newstring
for i in range(0,len(fileinput)):
    fileinput[i] = commentstripper(fileinput[i])
numpattern = re.compile('(\d)+(\.\d+)?([E][-+]?\d+)?')
wordpattern = re.compile('[a-zA-Z]+')
symbolpattern = re.compile('(<=)|(>=)|(==)|(!=)|(>)|(<)|(/)|(\*)|({)|(})|(\+)|(-)|(;)|(,)|(\()|(\))|(\[)|(\])|(=)')
def popnum(mystr, num):
    mystr = mystr.replace(num, '', 1)
    if ("." in num or "E" in num):
        lexemelist.append(lexeme("float", num))
    else:
        lexemelist.append(lexeme("num", num))
    return mystr
def popword(mystr, word):
    mystr = mystr.replace(word, '', 1)
    if (word in legalkeywords):
        lexemelist.append(lexeme("keyword", word))
    else:
        lexemelist.append(lexeme("id", word))
    return mystr
def popsymbol(mystr, symbol):
    mystr = mystr.replace(symbol, '', 1)
    lexemelist.append(lexeme("symbol", symbol))
    return mystr
for i in range(0,len(fileinput)):
    num = numpattern.search(fileinput[i])
    word = wordpattern.search(fileinput[i])
    symbol = symbolpattern.search(fileinput[i])
    while(num or word or symbol):
        wordindex = 0
        symbolindex = 0
        numindex = 0
        min = 10000
        mintype = ''
        if num:
            numindex = fileinput[i].find(num.group())
            if numindex < min:
                min = numindex
                mintype = 'num'
        if word:
            wordindex = fileinput[i].find(word.group())
            if wordindex < min:
                min = wordindex
                mintype = 'word'
        if symbol:
            symbolindex = fileinput[i].find(symbol.group())
            if symbolindex < min:
                min = symbolindex
                mintype = 'symbol'
        if mintype == 'num':
            fileinput[i] = popnum(fileinput[i],num.group())
        if mintype == 'word':
            fileinput[i] = popword(fileinput[i],word.group())
        if mintype == 'symbol':
            fileinput[i] = popsymbol(fileinput[i], symbol.group())
        num = numpattern.search(fileinput[i])
        symbol = symbolpattern.search(fileinput[i])
        word = wordpattern.search(fileinput[i])
    leftovers = fileinput[i].split()
    if leftovers:
        for i in range(0,len(leftovers)):
            
            sys.exit()
for i in range(0,len(lexemelist)):
    if lexemelist[i].type == 'keyword' or lexemelist[i].type == 'symbol':
        tokens.append(lexemelist[i].value)
    elif lexemelist[i].type == 'float' or lexemelist[i].type == 'num':
        tokens.append('num')
    else:
        tokens.append(lexemelist[i].type)
tokens.append("$")
####################################################END OF LEXICAL ANALYSER#######################################################

namei = 0

backlog = []

tempq = 0
qs = []

paramtally = 0

def clearBacklog():
    global backlog
    global tempq
    global qs
    global namei

    #print("backlog: " + str(backlog))

    if tempq != 0:
        sent = tempq.bpsent
        sendval = tempq.bpsendval
    else:
        sent = True
        sendval = -1

    for i in range(len(backlog)):
        if backlog[i] == "__call2__":
            tally = 0
            for q in range(len(qs)-1,-1,-1):
                if qs[q].one !="arg":
                    break
                else:
                    tally += 1
            qs.append(quadruple("call",backlog[i-1],str(tally),"_t"+str(namei)))
            backlog[i-1:i+1] = ["_t" + str(namei)]
            namei += 1
            clearBacklog()
            break

    for i in range(len(backlog)):
        if backlog[i] == "__call__":
            qs.append(quadruple("call",backlog[i+1],"1","_t" + str(namei)))
            backlog[i:i+2] = ["_t" + str(namei)]
            namei +=1
            clearBacklog()
            break

    for i in range(len(backlog)):
        if backlog[i] == "*":
            qs.append(quadruple("mult",backlog[i-1],backlog[i+1],"_t" + str(namei),True,-1,sent,sendval))
            tempq = 0
            backlog[i-1:i+2] = ["_t" + str(namei)]
            namei += 1
            clearBacklog()
            break
        if backlog[i] == "/":
            qs.append(quadruple("div",backlog[i-1],backlog[i+1],"_t" + str(namei),True,-1,sent,sendval))
            tempq = 0
            backlog[i-1:i+2] = ["_t" + str(namei)]
            namei += 1
            clearBacklog()
            break
    for i in range(len(backlog)):
        if backlog[i] == "+":
            qs.append(quadruple("add",backlog[i-1],backlog[i+1],"_t" + str(namei),True,-1,sent,sendval))
            tempq = 0
            backlog[i-1:i+2] = ["_t" + str(namei)]
            namei += 1
            clearBacklog()
            break
        if backlog[i] == "-":
            qs.append(quadruple("sub",backlog[i-1],backlog[i+1],"_t" + str(namei),True,-1,sent,sendval))
            tempq = 0
            backlog[i-1:i+2] = ["_t" + str(namei)]
            namei += 1
            clearBacklog()
            break
    for i in range(len(backlog)):
        if backlog[i] == "=":
            qs.append(quadruple("assign",backlog[i+1],"",backlog[i-1],True,-1,sent,sendval))
            tempq = 0
            del backlog[i-1:i+1]
            clearBacklog()
            break
    for i in range(len(backlog)):
        if backlog[i] in ["==","<","<=",">",">=","!="]:
            tempop = backlog[i]
            qs.append(quadruple("comp",backlog[i-1],backlog[i+1],"_t" + str(namei)))
            backlog[i-1:i+2] = ["_t" + str(namei)]
            if tempop == ">":
                qs.append(quadruple("BRLEQ",backlog[i-1],"",-1,False))
            elif tempop == ">=":
                qs.append(quadruple("BRL", backlog[i - 1], "", -1, False))
            elif tempop == "<=":
                qs.append(quadruple("BRG", backlog[i - 1], "", -1, False))
            elif tempop == "<":
                qs.append(quadruple("BRGEQ", backlog[i - 1], "", -1, False))
            elif tempop == "!=":
                qs.append(quadruple("BREQ", backlog[i - 1], "", -1, False))
            elif tempop == "==":
                qs.append(quadruple("BRNEQ", backlog[i - 1], "", -1, False))
            namei += 1
            clearBacklog()
            break
    for i in range(len(backlog)):
        if backlog[i] == "return":
            if i < len(backlog) - 1:
                qs.append(quadruple("return","","",backlog[i+1]))
            else:
                qs.append(quadruple("return","","",""))

    backlog = []

class tempquadruple:
    id = ""
    type = ""
    returnval = ""
    bpsendval = -1
    bpsent = False
    bprecval = -1
    bprecd = False
    def __init__(self,id,type,returnval):
        self.id = id
        self.type = type
        self.returnval = returnval

class quadruple:
    one = ""
    two = ""
    three = ""
    four = ""
    bpsendval = -1
    bpsent = True
    bprecval = -1
    bprecd = True

    def __init__(self,one,two,three,four,bprecd=True,bprecval=-1,bpsent=True,bpsendval=-1):
        self.one = one
        self.two = two
        self.three = three
        self.four = four
        self.bprecd = bprecd
        self.bpsent = bpsent
        self.bpsendval = bpsendval
        self.bprecval = bprecval

#Just so that 0 is taken
qs.append(quadruple("","","",""))

def A():
    validTokens1 = ["int", "void", "float"]
    if (tokens[0] in validTokens1):
        stack.extend(["B"])
    else:
        
        sys.exit()


def B():
    validTokens1 = ["int", "void", "float"]
    if (tokens[0] in validTokens1):
        stack.extend(["B'","C"])
    else:
        
        sys.exit()


def Bprime():
    validTokens1 = ["int", "void", "float"]
    if (tokens[0] in validTokens1):
        stack.extend(["B'","C"])
    elif (tokens[0] == "$"):
        x = 0
    else:
        
        sys.exit()


def C():
    validTokens1 = ["int", "void", "float"]
    if (tokens[0] in validTokens1):
        stack.extend(["C'","id","F"])
    else:
        
        sys.exit()


def Cprime():
    if (tokens[0] == "("):
        stack.extend(["E"])
        global tempq
        if tempq.type == "void":
            tempint = 0
        else:
            tempint = 1
        qs.append(quadruple("func",tempq.id,tempq.type,tempint,""))
        tempq = 0
    elif (tokens[0] == "["):
        stack.extend(["D"])
    elif (tokens[0] == ";"):
        stack.extend(["D"])
    else:
        
        sys.exit()


def D():
    if (tokens[0] == "["):
        stack.extend([";","]","num","["])
    elif (tokens[0] == ";"):
        stack.extend([";"])
        global tempq
        if tempq != 0:
            qs.append(quadruple("alloc","4","",tempq.id))
            tempq = 0
    else:
        
        sys.exit()


def E():
    if (tokens[0] == "("):
        stack.extend(["funcend","H",")","G","("])
    else:
        
        sys.exit()


def F():
    if (tokens[0] in ["int", "void", "float"]):
        stack.extend([tokens[0]])
        global tempq
        tempq = tempquadruple("",tokens[0],"")
        if lexemelist[1].type == "id":
            tempq.id = lexemelist[1].value
        else:
            sys.exit()
    else:
        
        sys.exit()


def G():
    global paramtally
    if (tokens[0] in  ["int","float"]):
        stack.extend(["I'","J'", "id", tokens[0]])
        if lexemelist[1].type == "id":
            tempid = lexemelist[1].value
        else:
            sys.exit()
        qs.append(quadruple("param","","",tempid))
        qs.append(quadruple("alloc","4","",tempid))
        paramtally += 1
    elif (tokens[0] == "void"):
        stack.extend(["G'","void"])
        #Think about possibly taking out G', because realistically a void param should not have any trailing params, not sure where I got it
    else:
        
        sys.exit()


def Gprime():
    if (tokens[0] == ")"):
        x = 0
    elif (tokens[0] == "id"):
        stack.extend(["I'","J'","id"])
    else:
        
        sys.exit()

def H():
    if (tokens[0] == "{"):
        stack.extend(["}","L","K","{"])
    else:
        
        sys.exit()


def I():
    validTokens1 = ["int", "void", "float"]
    if (tokens[0] in validTokens1):
        stack.extend(["I'", "J"])
    else:
        
        sys.exit()


def Iprime():
    global qs
    global paramtally
    if (tokens[0] == ","):
        stack.extend(["I'","J",","])
    elif (tokens[0] == ")"):
        for i in range(len(qs)-1,-1,-1):
            if qs[i].one == "func":
                qs[i].four = paramtally
                paramtally = 0
                break
    else:
        
        sys.exit()


def J():
    global paramtally
    validTokens1 = ["int", "void", "float"]
    if (tokens[0] in validTokens1):
        stack.extend(["J'", "id", "F"])
        if lexemelist[1].type == "id":
            tempid = lexemelist[1].value
        else:
            sys.exit()
        qs.append(quadruple("param","","",tempid))
        paramtally += 1
    else:
        
        sys.exit()


def Jprime():
    if (tokens[0] == ")" or tokens[0] == ","):
        global tempq
        if tempq != 0:
            qs.append(quadruple("alloc","4","",tempq.id))
            tempq = 0
    elif (tokens[0] == "["):
        stack.extend(["]","["])
    else:
        
        sys.exit()


def K():
    validTokens1 = ["int", "void", "float"]
    validTokens2 = ["(", "{", "}", ";", "id", "if", "return", "while", "num"]
    if (tokens[0] in validTokens1):
        stack.extend(["K'"])
    elif (tokens[0] in validTokens2):
        x = 0
    else:
        
        sys.exit()


def Kprime():
    validTokens1 = ["int", "void", "float"]
    validTokens2 = ["(", "{", "}", ";", "id", "if", "return", "while", "num"]
    if (tokens[0] in validTokens1):
        stack.extend(["K'","D","id","F"])
    elif (tokens[0] in validTokens2):
        x = 0
    else:
        
        sys.exit()


def L():
    validTokens1 = ["(", "{", "}", ";", "id", "if", "return", "while", "num"]
    if (tokens[0] in validTokens1):
        stack.extend(["L'"])
    else:
        
        sys.exit()


def Lprime():
    validTokens1 = ["(","{",";", "id", "if", "return", "while", "num"]
    if (tokens[0] in validTokens1):
        stack.extend(["L'","M"])
    elif (tokens[0] == "}"):
        x=0
    else:
        
        sys.exit()

def M():
    validTokens1 = ["(",";","id","num"]
    if (tokens[0] in validTokens1):
        stack.extend(["N"])
    elif (tokens[0] == "{"):
        stack.extend(["blockend", "H"])
        qs.append(quadruple("block", "", "", ""))
    elif (tokens[0] == "if"):
        stack.extend(["elseend", "M", "else", "ifend", "M",")","R","(","if"])
    elif (tokens[0] == "return"):
        stack.extend(["Q"])
    elif (tokens[0] == "while"):
        stack.extend(["P"])
    else:
        
        sys.exit()

def N():
    validTokens1 = ["(","id","num"]
    if (tokens[0] in validTokens1):
        stack.extend([";", "R"])
    elif (tokens[0] == ";"):
        stack.extend([";"])
        clearBacklog()
    else:
        
        sys.exit()

def P():
    if (tokens[0] == "while"):
        stack.extend(["whileend","M",")","R","(","while"])
        global tempq
        global qs
        tempq = tempquadruple("","","")
        tempq.bpsendval = len(qs) + 1
        tempq.bpsent = False
    else:
        
        sys.exit()

def Q():
    if (tokens[0] == "return"):
        stack.extend(["Q'", "return"])
        backlog.append("return")
    else:
        
        sys.exit()

def Qprime():
    validTokens1 = ["(", "id", "num"]
    if (tokens[0] in validTokens1):
        stack.extend([";", "R"])
    elif (tokens[0] == ";"):
        stack.extend([";"])
        clearBacklog()
    else:
        
        sys.exit()

def R():
    if (tokens[0] in ["(", "num"]):
        stack.extend(["fakeT"])
    elif (tokens[0] == "id"):
        stack.extend(["R'","id"])
        backlog.append(lexemelist[0].value)
    else:
        
        sys.exit()

def Rprime():
    if (tokens[0] in [")", "[", "]", ";", ","]):
        stack.extend(["S''","S'"])
        x=0
    elif (tokens[0] == "("):
        stack.extend(["U'", "V'", "X'",")", "~", "("])
        backlog.append("__call2__")
    elif (tokens[0] == "="):
        stack.extend(["R","="])
        backlog.append(tokens[0])
    elif (tokens[0] in ["<", "<=", ">", ">=", "==", "!="]):
        stack.extend(["U'"])
        x=0
    elif (tokens[0] in ["+", "-"]):
        stack.extend(["U'", "V'"])
    elif (tokens[0] in ["/", "*"]):
        stack.extend(["U'", "V'", "X'"])
    else:
        
        sys.exit()

def fakeT():
    if (tokens[0] == "("):
        stack.extend(["U'", "V'", "X'",")", "R", "("])
    elif (tokens[0] == "num"):
        stack.extend(["U'", "V'", "X'","num"])
        backlog.append(lexemelist[0].value)
    else:
        
        sys.exit()

def S():
    if (tokens[0] == "id"):
        stack.extend(["S'", "id"])
    else:
        
        sys.exit()

def Sprime():
    validTokens1 = [")", "]", ";", ",", "+", "-", "<", "<=", ">", ">=", "==", "!=",  "/","*","="]
    if (tokens[0] in validTokens1):
        x = 0
    elif (tokens[0] == "["):
        stack.extend(["]","R","["])
    else:
        
        sys.exit()

def Sdoubleprime():
    if (tokens[0] in [")", "]", ";", ",", "+", "-", "<", "<=", ">", ">=", "==", "!=",  "/","*"]):
        x=0
    elif (tokens[0] == "="):
        stack.extend(["R","="])
        backlog.append(tokens[0])

def T():
    validTokens1 = ["(", "id", "num"]
    if (tokens[0] in validTokens1):
        stack.extend(["T'", "V"])
    else:
        
        sys.exit()

def Tprime():
    validTokens1 = ["<=", "<", ">", ">=", "==", "!="]
    if (tokens[0] in validTokens1):
        stack.extend(["V", "U"])
    else:
        
        sys.exit()

def U():
    reduceTokens = ["<=", "<", ">", ">=", "==", "!="]
    if (tokens[0] in reduceTokens):
        stack.extend([tokens[0]])
        backlog.append(tokens[0])
    else:
        
        sys.exit()

def Uprime():
    if (tokens[0] in [")", "]", ";", ","]):
        clearBacklog()
    elif (tokens[0] in ["<", "<=", ">", ">=", "==", "!="]):
        stack.extend(["V","U"])


def V():
    validTokens1 = ["(", "id", "num"]
    if (tokens[0] in validTokens1):
        stack.extend(["V'", "X"])
    else:
        
        sys.exit()

def Vprime():
    validTokens1 = ["<", "<=", ">", ">=", "==", "!="]
    if (tokens[0] in validTokens1):
        x=0
    elif (tokens[0] in [")","]",";",","]):
        clearBacklog()
    elif (tokens[0] == "+" or tokens[0] == "-"):
        stack.extend(["V'", "X", "W"])
    else:
        
        sys.exit()

def W():
    reduceTokens = ["+", "-"]
    if (tokens[0] in reduceTokens):
        stack.extend([tokens[0]])
        backlog.append(tokens[0])
    else:
        
        sys.exit()

def X():
    validTokens1 = ["(", "id", "num"]
    if (tokens[0] in validTokens1):
        stack.extend(["X'", "Z"])
    else:
        
        sys.exit()

def Xprime():
    validTokens1 = ["/", "*"]
    if (tokens[0] in validTokens1):
        stack.extend(["X'","Z","Y"])
    elif (tokens[0] in [")","]",";",",","+","-", "<", "<=", ">", ">=", "==", "!="]):
        x = 0
    else:
        
        sys.exit()

def Y():
    reduceTokens = ["/", "*"]
    if (tokens[0] in reduceTokens):
        stack.extend([tokens[0]])
        backlog.append(tokens[0])
    else:
        
        sys.exit()

def Z():
    global backlog
    if (tokens[0] == "("):
        stack.extend([")", "R", "("])
    elif (tokens[0] == "id"):
        stack.extend(["Z'", "id"])
        backlog.append(lexemelist[0].value)
    elif (tokens[0] == "num"):
        stack.extend(["num"])
        backlog.append(lexemelist[0].value)
    else:
        
        sys.exit()

def Zprime():
    validTokens1 = [")","[","]",";",",","+","-","*","/", "<", "<=", ">", ">=", "==", "!="]
    if (tokens[0] in validTokens1):
        stack.extend(["S'"])
    elif (tokens[0] == "("):
        stack.extend([")", "~", "("])
    else:
        
        sys.exit()

def Zdoubleprime():
    if (tokens[0] == "("):
        stack.extend([")","~","("])
    elif (tokens[0] == "num"):
        stack.extend(["num"])
        global backlog
        backlog.append(lexemelist[0].value)

def percent():
    if (tokens[0] == "id"):
        stack.extend([")","~","(","id"])
        backlog.append(lexemelist[0].value)
        backlog.append("__call__")
    else:
        
        sys.exit()

def squiggle():
    validTokens1 = ["(","id","num"]
    if (tokens[0] in validTokens1):
        stack.extend(["&"])
    elif (tokens[0] == ")"):
        x = 0
    else:
        
        sys.exit()

def ampersand():
    validTokens1 = ["(", "id", "num"]
    if (tokens[0] in validTokens1):
        stack.extend(["&'","R","arg"])
    else:
        
        sys.exit()

def ampersandprime():
    if (tokens[0] == ")"):
        x = 0
    elif (tokens[0] == ","):
        stack.extend(["&'","R","arg",","])
    else:
        
        sys.exit()


###As long as the stack is not empty
while (stack):
    #print("stack: " + str(stack))
    #print("Tokens: " + str(tokens[:15]))
    #yeet = input()
    x = stack.pop()
    ###If the item on top of the stack is A, call the A function, etc.
    if (x == "A"):
        A()
    elif (x == "B"):
        B()
    elif (x == "B'"):
        Bprime()
    elif (x == "C"):
        C()
    elif (x == "C'"):
        Cprime()
    elif (x == "D"):
        D()
    elif (x == "E"):
        E()
    elif (x == "F"):
        F()
    elif (x == "G"):
        G()
    elif (x == "G'"):
        Gprime()
    elif (x == "H"):
        H()
    elif (x == "I"):
        I()
    elif (x == "I'"):
        Iprime()
    elif (x == "J"):
        J()
    elif (x == "J'"):
        Jprime()
    elif (x == "K"):
        K()
    elif (x == "K'"):
        Kprime()
    elif (x == "L"):
        L()
    elif (x == "L'"):
        Lprime()
    elif (x == "M"):
        M()
    elif (x == "N"):
        N()
    elif (x == "P"):
        P()
    elif (x == "Q"):
        Q()
    elif (x == "Q'"):
        Qprime()
    elif (x == "R"):
        R()
    elif (x == "R'"):
        Rprime()
    elif (x == "fakeT"):
        fakeT()
    elif (x == "S"):
        S()
    elif (x == "S'"):
        Sprime()
    elif (x == "S''"):
        Sdoubleprime()
    elif (x == "T"):
        T()
    elif (x == "T'"):
        Tprime()
    elif (x == "U"):
        U()
    elif (x == "U'"):
        Uprime()
    elif (x == "V"):
        V()
    elif (x == "V'"):
        Vprime()
    elif (x == "W"):
        W()
    elif (x == "X"):
        X()
    elif (x == "X'"):
        Xprime()
    elif (x == "Y"):
        Y()
    elif (x == "Z"):
        Z()
    elif (x == "Z'"):
        Zprime()
    elif (x == "Z''"):
        Zdoubleprime()
    elif (x == "%"):
        percent()
    elif (x == "~"):
        squiggle()
    elif (x == "&"):
        ampersand()
    elif (x == "&'"):
        ampersandprime()
    elif(x=="blockend"):
        qs.append(quadruple("end", "block", "", ""))
    elif(x=="whileend"):
        for i,q in reversed(list(enumerate(qs))):
            if q.bpsent == False:
                qs.append(quadruple("BR","","",str(i)))
                q.pbsent = True
                for j in range(i,len(qs)):
                    if qs[j].bprecd == False:
                        qs[j].four = len(qs)
                        qs[j].bprecd = True
                        break
                break
    elif(x=="funcend"):
        for q in reversed(qs):
            if q.one == "func":
                qs.append(quadruple("end", "func", q.two, ""))
                break

    elif(x=="ifend"):
        qs.append(quadruple("BR","","",""))
        for i,q in reversed(list(enumerate(qs))):
            if q.bpsent == False:
                qs.append(quadruple("BR","","",str(i)))
                q.pbsent = True
                break

    elif(x=="elseend"):
        #NO "BR" QUADRUPLE, ONLY TELL THE "IF" BREAK WHERE TO GO IF CONDITION NOT MET
        x=0

    elif(x=="arg"):
        qs.append(quadruple("arg","","",str(lexemelist[0].value)))

    ###If the item on top of the stack and the next token are the same, pop em both
    elif (x ==  tokens[0]):
        ###If there is only one item ($) in both the stack and the list of tokens, ACCEPT
        if (x == "$" and len(tokens) == 1):
            for i in range(1,len(qs)):
                print(str(i) + ": " + str(qs[i].one).ljust(8, " ") + str(qs[i].two).ljust(8, " ") + str(
                    qs[i].three).ljust(8, " ") + str(qs[i].four).ljust(8, " "))
            sys.exit()
        else:
            #print(tokens.pop(0))
            tokens.pop(0)
            lexemelist.pop(0)

    elif (x == "else"):
        if (stack.pop() == "M"):
            if (stack.pop() == "elseend"):
                continue

    ###If the item on top of the stack is not a valid terminal/non-terminal, REJECT
    else:
        
        sys.exit()