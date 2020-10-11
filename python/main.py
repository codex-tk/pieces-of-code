
import codecs

with codecs.open("./in", encoding='utf-8') as file:
    for line in file.readlines():
        print(line[line.find('(')+1:line.find(')')])
