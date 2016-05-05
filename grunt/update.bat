@echo off
java org.antlr.Tool grunt.g -verbose
java org.antlr.Tool gruntEval.g -verbose
copy gruntLexer.c gruntLexer.cpp /Y
copy gruntParser.c gruntParser.cpp /Y
copy gruntEval.c gruntEval.cpp /Y
del gruntLexer.c
del gruntParser.c
del gruntEval.c