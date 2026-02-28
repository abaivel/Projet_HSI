from pandas import read_excel, isna

def get_min_max_from_interval(interval):
    interval_trimed = interval[1:-1]
    min_s, max_s = interval_trimed.split(";")
    return min_s,max_s


types = read_excel("data_project.xlsx", "Types")
data = read_excel("data_project.xlsx", "Data")


c_file = open("bcgv_lib.c", "w")
h_file = open("bcgv_lib.h", "w")

h_file.write("#ifndef BCGV_LIB_H_\n#define BCGV_LIB_H_\n\n#include <stdint.h>\n\n")
h_file.write("#define SER_NUM_BGF 11\n#define SER_NUM_COMODO 12\n\n")

c_file.write('#include "bcgv_lib.h"\n\n')

dimensions= {}

for t in types.values:
    if t[1]=="Atom":
        h_file.write("typedef "+t[2]+" "+t[0]+";\n")
        if not isna(t[3]):
            dimensions[t[0]] = get_min_max_from_interval(t[3])
    elif t[1]=="enum":
        h_file.write("\n"+t[2]+"\n")

h_file.write("\n")

for d in data.values:
    c_file.write("static "+d[2]+" "+d[1]+" = "+str(d[4])+";\n")
    h_file.write(d[2]+" get_"+d[1]+"();\n")
    h_file.write("void set_"+d[1]+"("+d[2]+" new_value"+");\n\n")

c_file.write("\n\n")

for d in data.values:
    get_f = d[2]+" get_"+d[1]+"(){\n"
    get_f += "  return "+d[1]+";\n"
    get_f += "}\n\n"
    c_file.write(get_f)

    set_f = "void set_"+d[1]+"("+d[2]+" new_value"+"){\n"

    if d[2] in dimensions.keys() :
        set_f += "  if (new_value >= "+dimensions[d[2]][0]+" && new_value <= "+dimensions[d[2]][1]+"){\n"
        set_f += "      "+d[1]+" = new_value;\n"
        set_f += "  }\n"
    else: 
        set_f += "  "+d[1]+" = new_value;\n"
    
    set_f += "}\n\n"

    c_file.write(set_f)




h_file.write("\n#endif")
c_file.close()
h_file.close()