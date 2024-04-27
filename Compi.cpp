#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <regex>
#include <conio.h>
#include <stack> 
#include <cstdlib>
using namespace std;

bool esPalabraReservada(const string& palabra) {
    unordered_set<string> palabrasReservadas = {
        "if", "else", "while", "for", "int", "float", "char", "return", "bool",
        "const", "true", "false", "break", "main", "cout", "endl","void", "switch", "case"
        ,"default","string","cin"
    };

    return palabrasReservadas.count(palabra) > 0;
}

bool esCaracterEspecial(char caracter) {
    string caracteresEspeciales = "!''@#$%<>^&*-+=:,.?/";
    return caracteresEspeciales.find(caracter) != string::npos;
}

bool esCaracterEspecialCierre(char caracter) {
    string caracteresEspecialesCierre = ")]";
    return caracteresEspecialesCierre.find(caracter) != string::npos;
}

bool esCaracterEspecialApertura(char caracter) {
    string caracteresEspecialesApertura = "[(";
    return caracteresEspecialesApertura.find(caracter) != string::npos;
}

bool esCaracterpuntocoma(char caracter){
	string caracterpuntocoma = ";";
	return caracterpuntocoma.find(caracter) !=string::npos;
}

bool esCaracterinicio(char caracter){
	string caracterinicio = "{";
	return caracterinicio.find(caracter) !=string::npos;
}

bool esCaracterfinal(char caracter){
	string caracterfinal = "}";
	return caracterfinal.find(caracter) !=string::npos;
}

bool esNumero(const string& palabra) {
    for (char caracter : palabra) {
        if (!isdigit(caracter) && caracter != '.') {
            return false;
        }
    }
    return true;
}

enum class TipoToken {
    PR,
    ID,
    CE,
    CP,
    CEI,
    CEF,
    COMENT,
    N,
    CADENA,
    INI,
    FIN,
    DESCONOCIDO
};

struct Token {
    string valor;
    TipoToken tipo;
    string tipoDato; // Para almacenar el tipo de dato en caso de ser una declaración
    string nombreVariable; // Para almacenar el nombre de la variable
    string valorVariable; // Agregado para almacenar el valor inicial si existe
};

unordered_map<string, string> variablesDeclaradas;
unordered_map<string, string> valoresDeVariables;
vector<string> mensajesParaImprimir;

//Semantico
void almacenarVariable(const string& tipo, const string& nombre) {
    // Guarda la variable en el mapa
    variablesDeclaradas[nombre] = tipo;
}

int obtenerValor(const string& operando) {
    // Verifica si el operando es un número
    if (!operando.empty() && isdigit(operando[0]) || (operando[0] == '-' && operando.size() > 1)) {
        return stoi(operando);
    }
    // Si no es un número, asume que es una variable y obtén su valor
    return stoi(valoresDeVariables[operando]);
}

int evaluarExpresion(string operando1, string operador, string operando2) {
    int num1 = obtenerValor(operando1);
    int num2 = obtenerValor(operando2);
    if (operador == "+") return num1 + num2;
    if (operador == "-") return num1 - num2;
    if (operador == "*") return num1 * num2;
    if (operador == "/") return num1 / num2;
}

bool evaluarCondicion(vector<Token>& tokens, size_t& i,int& Error) {
    // Asumimos que 'i' está en la posición del '(' que abre la condición
    i++; // Avanza para apuntar al primer token de la condición

    string operando1, operador, operador1="vacio", operando2;

    // Obten el primer operando
    if (tokens[i].tipo == TipoToken::ID || tokens[i].tipo == TipoToken::N) {
        operando1 = tokens[i].valor;
        i++;
        cout<<"Operando1: "<<operando1<<endl;
    } else {
        cout<<"Error: Operando 1 if mal estructurado"<<endl;
        Error=2;
        return false;
    }

    // Obten el operador
    if (tokens[i].tipo == TipoToken::CE) { // Asumiendo que CE representa operadores de comparación
        operador = tokens[i].valor;
        i++;
         cout<<"Operador: "<<operador<<endl;
         	 if (tokens[i].tipo == TipoToken::CE) {
       			 operador1 = tokens[i].valor;
       			 	i++;
        			cout<<"Operador1: "<<operador1<<endl;
   			 }
    } else {
        cout<<"Error: Operador 1 if mal estructurado"<<endl;
        Error= 2;
        return false;
    }

    // Obten el segundo operando
    if (tokens[i].tipo == TipoToken::ID || tokens[i].tipo == TipoToken::N) {
        operando2 = tokens[i].valor;
        i++;
        cout<<"Operando2: "<<operando2<<endl;
    } else{
        cout<<"Error: Operando 2 if mal estructurado"<<endl;
        Error= 2;
        return false;
    }

    // Aumentar 'i' para saltar el ')' que cierra la condición
    i++;

    // Evaluar la condición
    int valor1 = esNumero(operando1) ? stoi(operando1) : stoi(valoresDeVariables[operando1]);
	int valor2 = esNumero(operando2) ? stoi(operando2) : stoi(valoresDeVariables[operando2]);
	if(operador1=="vacio"){
		cout << "Comparando: " << valor1 << " " << operador << " " << valor2 << endl;
	}
	if(operador1=="vacio" && (operador=="="||operador=="!")){
		cout<<"If mal estructurado"<<endl;
		Error= 2;
        return false;
	}
	cout << "Comparando: " << valor1 << " " << operador << "" <<operador1<<" "<< valor2 << endl;

    if (operador == "=" && operador1== "=") return valor1 == valor2;
    if (operador == "!" && operador1== "=") return valor1 != valor2;
    if (operador == "<") return valor1 < valor2;
    if (operador == ">") return valor1 > valor2;
    if (operador == "<" && operador1== "=") return valor1 <= valor2;
    if (operador == ">" && operador1== "=") return valor1 >= valor2;

    // Si el operador no es reconocido, devuelve false o maneja el error como prefieras
    return false;
}

bool evaluarCondicionFor(const string& variable, const string& operador, const string& valorFinal) {
    cout<<"Evaluar condicion"<<endl;
	int valorVariable = stoi(valoresDeVariables[variable]);
    int valorFinalInt = stoi(valorFinal);

    if (operador == "<=") {
        return valorVariable <= valorFinalInt;
    } else if (operador == "<") {
        return valorVariable < valorFinalInt;
    } else if (operador == ">=") {
        return valorVariable >= valorFinalInt;
    } else if (operador == ">") {
        return valorVariable > valorFinalInt;
    }
    // Agrega aquí más operadores según sea necesario

    return false;
}

void procesarCout(const vector<Token>& tokens, size_t& i, vector<string>& mensajes) {
    i++; // Avanza al siguiente token después de cout
    string mensaje = "";

    while (i < tokens.size() && tokens[i].tipo != TipoToken::CP) {
        if (tokens[i].tipo == TipoToken::CADENA) {
            mensaje += tokens[i].valor.substr(1, tokens[i].valor.length() - 2);
        } else if (tokens[i].tipo == TipoToken::ID) {
            string nombreVariable = tokens[i].valor;
            if (valoresDeVariables.find(nombreVariable) != valoresDeVariables.end()) {
                mensaje += valoresDeVariables[nombreVariable];
            } else {
                mensaje += "undefined";
            }
        }

        i++;

        if (i < tokens.size() && tokens[i].tipo == TipoToken::CE && tokens[i].valor == "<<") {
            i++;
        }
    }

    mensajes.push_back(mensaje);
}

void ejecutarFor(const string& variable, const string& valorInicializacion, 
                 const string& condicion, const string& valorFinal, 
                 const string& incremento,const vector<Token>& tokens) {
    // Establecer la variable de inicialización
    cout << "Ejecutando for" << endl;
    valoresDeVariables[variable] = valorInicializacion;

    // Ejecutar el bucle for
    while (evaluarCondicionFor(variable, condicion, valorFinal)) {
        // Aquí es donde puedes ejecutar cualquier lógica específica para cada iteración del bucle.
        // Por ejemplo, si deseas imprimir algo en cada iteración, puedes hacerlo aquí.
        cout << "Iteracion del for con " << variable << " = " << valoresDeVariables[variable] << endl;
        // Aquí puedes agregar más lógica si es necesario
		for (size_t i = 0; i < tokens.size(); ++i) {
            const Token& token = tokens[i];
            if (token.tipo == TipoToken::PR && token.valor == "cout") {
                procesarCout(tokens, i, mensajesParaImprimir);
                // Asegúrate de actualizar 'i' dentro de procesarCout si es necesario
            }
            // Manejar otros tipos de tokens según sea necesario
        }
        // Actualizar la variable del bucle según el incremento/decremento
        if (incremento == "++") {
            valoresDeVariables[variable] = to_string(stoi(valoresDeVariables[variable]) + 1);
        } else if (incremento == "--") {
            valoresDeVariables[variable] = to_string(stoi(valoresDeVariables[variable]) - 1);
        }
    }
}

string tokenTypeToRegex(TipoToken tipo) {
    switch (tipo) {
        case TipoToken::PR: return "PR";
        case TipoToken::ID: return "ID";
        case TipoToken::CE: return "CE";
        case TipoToken::CP: return "CP";
        case TipoToken::CEI: return "CEI";
        case TipoToken::CEF: return "CEF";
        case TipoToken::CADENA: return "CADENA";
        case TipoToken::COMENT: return "COMENT";
        case TipoToken::N: return "N";
        case TipoToken::INI: return "INI";
        case TipoToken::FIN: return "FIN";
        default: return "DESCONOCIDO";
    }
}

vector<Token> analizarLexico(const string& codigo) {
    vector<Token> tokens;
    string palabraActual = "";
    bool enComentarioBloque = false;
    string comentarioActual = "";
    bool enCadena = false;
    string cadenaActual = "";

    for (size_t i = 0; i < codigo.size(); i++) {
        char caracter = codigo[i];
        
        // Manejo de comentarios de bloque
        if (enComentarioBloque) {
            comentarioActual += caracter;
            if (i < codigo.size() - 1 && codigo[i] == '*' && codigo[i + 1] == '/') {
                enComentarioBloque = false;
                comentarioActual += '/';
                tokens.push_back({comentarioActual, TipoToken::COMENT});
                comentarioActual = "";
                i++; // Salta el '*'
                continue; // Continúa 
            }
            continue; // Continúa añadiendo caracteres al comentario
        } else if (!enComentarioBloque && i < codigo.size() - 1 && codigo[i] == '/' && codigo[i + 1] == '*') {
            enComentarioBloque = true;
            comentarioActual = "/*";
            i++; // Salta el '*' del inicio del comentario de bloque
            continue; // Continúa con el próximo ciclo
        }

        // Manejo de cadenas
        if (caracter == '"' && !enCadena) {
            enCadena = true;
            cadenaActual += caracter;
            continue;
        }
        if (enCadena) {
            cadenaActual += caracter;
            if (caracter == '"') {
                enCadena = false;
                tokens.push_back({cadenaActual, TipoToken::CADENA});
                cadenaActual = "";
            }
            continue;
        }

        // Manejo de comentarios de línea //
        if (!enComentarioBloque && !enCadena && i < codigo.size() - 1 && codigo[i] == '/' && codigo[i + 1] == '/') {
            // Ignora la linea
            while (i < codigo.size() && codigo[i] != '\n') {
                i++;
            }
            continue;
        }

        // Manejo de los demas tokens
        if (isalnum(caracter) || caracter == '_') {
            palabraActual += caracter;
        } else {
            if (!palabraActual.empty()) {
                if (esPalabraReservada(palabraActual)) {
                    tokens.push_back({palabraActual, TipoToken::PR});
                } else if (esNumero(palabraActual)) {
                    tokens.push_back({palabraActual, TipoToken::N});
                } else {
                    tokens.push_back({palabraActual, TipoToken::ID});
                }
                palabraActual = "";
            }

            if (esCaracterEspecial(caracter)) {
                tokens.push_back({string(1, caracter), TipoToken::CE});
            } else if (esCaracterpuntocoma(caracter)) {
                tokens.push_back({string(1, caracter), TipoToken::CP});
            } else if (esCaracterEspecialApertura(caracter)) {
                tokens.push_back({string(1, caracter), TipoToken::CEI});
            } else if (esCaracterEspecialCierre(caracter)) {
                tokens.push_back({string(1, caracter), TipoToken::CEF});
            }else if (esCaracterinicio(caracter)) {
                tokens.push_back({string(1, caracter), TipoToken::INI});
            }else if (esCaracterfinal(caracter)) {
                tokens.push_back({string(1, caracter), TipoToken::FIN});
            }
        }
    }

    // Manejo de la última palabra o número
    if (!palabraActual.empty()) {
        if (esPalabraReservada(palabraActual)) {
            tokens.push_back({palabraActual, TipoToken::PR});
        } else if (esNumero(palabraActual)) {
            tokens.push_back({palabraActual, TipoToken::N});
        } else {
            tokens.push_back({palabraActual, TipoToken::ID});
        }
    }

    return tokens;
}

void analizarSintaxis(const string& codigo, int& Error, int& contadorCorchetes) {
    vector<Token> tokens = analizarLexico(codigo);
    string tokensStr = "";
	int corchetesAbiertos = 0;
	bool dentroDeBloqueIf = false;
	bool dentroDeBloqueElse = false;
	
	for (size_t i = 0; i < tokens.size(); i++) {
        tokensStr += tokenTypeToRegex(tokens[i].tipo);
        if (i != tokens.size() - 1) {  // Si no es el último token, añade un espacio
            tokensStr += " ";
        }
    }

    cout << "Tokens generados: \"" << tokensStr << "\"" << endl; // Depuración
    // Definición de patrones específicos
    regex patron1("^PR\\sID\\s*CE\\s*N\\s*CP$");		// int variable=1;
    regex patron2("^PR\\sID\\s*CP$");					// int a;
    regex patron3("^PR\\sID(\\s*CE\\sID)*\\sCP$");		// int a,v,,f,f,d,...;
    regex patron4("^PR\\sPR\\s*CEI\\s*CEF\\s*INI$");	//Funcion
    regex patron5("^\\s*FIN\\s*");						// } del final
    regex patron6("^\\s*$");								//LINEA EN BLANCO
	regex patron7("^PR\\sID\\s*CEI\\s*PR\\sID\\s*CEF\\s*CP\\s*"); //void variable(int x);
	regex patron8("^PR\\sID\\s*CEI\\s*CEF\\s*INI\\s*");	//void variable(){
	regex patron9("^PR\\sID\\s*CEI\\s*\\PR\\sID(\\s*CE\\s*PR\\sID\\s*)*CEF\\s*CP\\s*"); // int suma(int a, int b);
	regex patron10("^PR\\sID\\s*CEI\\s*ID(\\s*CE\\s*ID\\s*)*CEF\\s*CP\\s*");//  int suma(a, b);
	regex patron11("^PR\\sPR\\sCEI\\s*PR\\sID\\s*CEF\\s*INI\\s*");// int obtenerValor(int parametro) {
	regex patron12("^PR\\sID\\s*CEI\\s*\\PR\\sID(\\s*CE\\s*PR\\sID\\s*)*CEF\\s*INI\\s*");// int suma(int a, int b) {
	regex patron13("^PR\\sID\\s*CEI\\s*ID(\\s*CE\\s*ID\\s*)*CEF\\s*INI\\s*"); //int suma(a, b) {
	regex patron15("^PR\\sID\\s*CEI\\s*CEF\\s*CP\\s*");//void imprimir();
	regex patron16("^PR\\sID\\s*CEI\\s*(PR\\sID(\\s*CE\\s*PR\\sID)*)?\\s*CEF\\s*CEI\\s*.*\\s*CEF$");//int suma(int a, int b) { return a + b; }
	regex patron17("^PR\\sID\\s*CEI\\s*PR\\s*CE\\s*ID(\\s*CE\\s*PR\\s*CE\\s*ID)*\\s*CEF\\s*CP$");//void procesar(int* arreglo, int& tamaño);
	regex patron18("^PR\\sID\\s*CEI\\s*PR\\s*ID\\s*CE\\s*N(\\s*CE\\s*PR\\s*ID\\s*CE\\s*N)*\\s*CEF\\s*CP$");//void establecerValores(int x = 10, int y = 20);
	regex patron19("^PR\\sID\\s*CEI\\s*(PR\\sID(\\s*CE\\s*PR\\sID)*)?\\s*CEF\\s*PR\\s*CP\\s*$");//int getValue() const;
	regex patron20("^ID\\s*CE\\s*(N|ID)\\s*CP$");//variable = valor; o variable = otraVariable;
	regex patron21("^PR\\s*CEI\\s*(ID|N)\\s*CE\\s*(CE\\s*)?(ID|N)\\s*CEF\\s*INI$");//if (condición) { y while(condicion){
	regex patron22("^CEF\\s*PR\\s*INI$");//Else
	regex patron23("^PR\\s*CEI\\s*ID\\s*CE\\s*N\\s*CP\\s*(ID|N)\\s*CE\\s*(CE\\s*)?(ID|N)\\s*CP\\s*ID\\s*CE\\s*(CE\\s*)?CEF\\s*INI$");//for (inicialización; condición; incremento) {
	regex patron24("^PR\\s*ID\\s*CEI\\s*N\\s*CEF\\s*CP$");//tipo nombre[tamaño]
	regex patron25("^PR\\s*CE\\s*ID\\s*CP$");//tipo* puntero;
	regex patron26("^PR\\s*ID\\s*CE\\s*(ID|N)\\s*CP$");//tipo vcariable = valor;
	regex patron27("^PR\\s*(ID|N)\\s*CP$");// return valor;
	regex patron28("^ID\\s*CEI\\s*CEF\\s*CP$");// nomnbreFuncion();
	regex patron29("^ID\\s*CEI\\s*(ID|N)(\\s*CE\\s*(ID|N))*\\s*CEF\\s*CP$");//nombrefuncion(parametro1,parametro2);
	regex patron30("^ID\\s*CE\\s*CE\\s*CP$");// variable++; o variable--;
	regex patron14("^COMENT$");
	//Textos
	regex patron31("^PR\\s+CE\\s+CE\\s+CADENA\\s+CP$");
	regex patron32("^ID\\s*CE\\s*CE\\s*ID\\s*CP$");
	regex patron33("^PR\\s*CE\\s*CE\\s*CADENA\\s*CE\\s*CE\\s*ID\\s*((CE\\s*CE\\s*CADENA\\s*CE\\s*CE\\s*ID\\s*)*)?CE\\s*CE\\s*PR\\s*CP$");
	regex patron34("^PR\\s*PR\\s*CEI\\s*CEF\\s*CP$");
	regex patron35("^PR\\s*CEI\\s*ID\\s*CE\\s*N\\s*CP\\s*(ID|N)\\s*CE\\s*(CE\\s*)?(ID|N)\\s*CP\\s*ID\\s*CE\\s*(CE\\s*)?CEF$");//For sin {
	regex patron36("^PR\\s*CEI\\s*PR\\s*ID\\s*CE\\s*N\\s*CP\\s*(ID|N)\\s*CE\\s*(CE\\s*)?(ID|N)\\s*CP\\s*ID\\s*CE\\s*(CE\\s*)?CEF$"); // For con int i
	regex patron37("^PR\\s+CEI\\s+PR\\s+ID\\s+CE\\s+N\\s+((CE\\s+ID\\s+CE\\s+N\\s+)*)?CP\\s+ID\\s+CE\\s+N\\s+CP\\s+ID\\s+CE\\s+CE\\s+((CE\\s+ID\\s+CE\\s+CE\\s+)*)?CEF$");//for con i=1,j=1--- i++,j++
	regex patron38("^PR\\s*CEI\\s*CP\\s*CP\\s*CEF$");//for raro for(&&)
	regex patron39("^PR\\s*ID\\s*CE\\s*PR\\s*CP$");// bool salir = false;
	regex patron40("^PR\\s*CEI\\s*CE\\s*ID\\s*CEF\\s*INI$");// while (!salir){
	regex patron41("^PR\\s*CE\\s*CE\\s*CADENA\\s*CE\\s*CE\\s*PR\\s*CP$");//cout<<"Hola"<<endl;
	regex patron42("^PR\\s*CEI\\s*ID\\s*CEF\\s*INI$"); //switch (opcion){
	regex patron43("^PR\\s*N\\s*CE$");// case 1
	regex patron44("^PR\\s*CP$");// break;
	regex patron45("^ID\\s*CE\\s*PR\\s*CP$");// salir = true;
	regex patron46("^PR\\s*CE$");//default:
	regex patron47("^PR\\s*ID\\s*CEI\\s*CEF\\s*CE\\s*CADENA\\s*CP$");// char cad[]="Hola";
	regex patron48("^PR\\s*CEI\\s*PR\\s*ID\\s*CE\\s*N\\s*CP\\s*ID\\s*CEI\\s*ID\\s*CEF\\s*CP\\s*ID\\s*CE\\s*CE\\s*CEF\\s*INI$");//for con cad[i]
	regex patron49("^PR\\s*CEI\\s*ID\\s*CEI\\s*ID\\s*CEF\\s*CEF\\s*INI$");//if con cad[i]{
	regex patron50("^PR\\s*CE\\s*CE\\s*CADENA\\s*CE\\s*CE\\s*ID\\s*CE\\s*CE\\s*CADENA\\s*CE\\s*CE\\s*ID\\s*CEI\\s*ID\\s*CEF\\s*CE\\s*CE\\s*CADENA\\s*CE\\s*CE\\s*PR\\s*CP$");
	regex patron51("^PR\\s*ID\\s*CEI\\s*PR\\s*ID\\s*CEF\\s*INI$"); // bool esPar(int numero) {
	regex patron52("^PR\\s*CEI\\s*ID\\s*CEI\\s*ID\\s*CEF\\s*CE\\s*ID\\s*CEI\\s*ID\\s*CE\\s*N\\s*CEF\\s*CEF$");// if con (funcion< funcion)
	regex patron53("^PR\\s*CEI\\s*ID\\s*CEI\\s*ID\\s*CEF\\s*CE\\s*ID\\s*CEI\\s*ID\\s*CE\\s*N\\s*CEF\\s*CE\\s*CE\\s*ID\\s*CEI\\s*ID\\s*CEF\\s*CE\\s*CE\\s*ID\\s*CEI\\s*N\\s*CEF\\s*CEF$");//if final
	regex patron54("^ID\\s*CE\\s*((ID\\s*CE\\s*)*)?ID\\s*CP$");
	regex patron55("^CE\\sCE$");
	regex patron56("^\\s*INI\\s*");
	regex patron57("^\\s*ID\\s*CE\\s*ID\\s*(CE\\s*N\\s*)?\\s*CP$");
	regex patron58("^\\s*PR\\s*ID\\s*CE\\s*CADENA\\s*CP$");
	regex patron59("^PR\\s*CE\\s*CE\\s*ID\\s*CP$");
	regex patron60("^PR\\s*CE\\s*CE\\s*CADENA\\s*CE\\s*CE\\s*ID\\s*CE\\s*CE\\s*CADENA\\s*CE\\s*CE\\s*PR\\s*CP$");
	regex patron61("^ID\\s*CE\\s*N\\s*CE\\s*N\\s*CP$");
	regex patron62("^PR\\s*ID\\s*CE\\s*N\\s*((CE\\s*ID\\s*CE\\s*N\\s*)*)?((CE\\s*ID\\s*)*)?CP$");
	regex patron63("^PR\\s*ID\\s*((CE\\s*ID\\s*CE\\s*N\\s*)*)?((CE\\s*ID\\s*)*)?CP$");
	regex patron64("^PR\\s*CE\\s*CE\\s*CADENA\\s*CE\\s*CE\\s*ID\\s*CE\\s*CE\\s*CADENA\\s*CP$");
    regex patron65("^FIN\\s*PR\\s*INI$");
    regex patron66("^PR\\s*INI$");
    regex patron67("^PR\\s*CEI\\s*PR\\s*ID\\s*CE\\s*N\\s*CP\\s*ID\\s*CE\\s*CE\\s*N\\s*CP\\s*ID\\s*CE\\s*CE\\s*CEF\\s*INI$");

    if (regex_match(tokensStr, patron1)) {
        cout << "Linea correcta" << endl;
    } else if (regex_match(tokensStr, patron2)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron3)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron4)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron5)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron8)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron7)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron9)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron10)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron11)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron12)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron13)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron14)){
    	cout << "COMENTARIO" << endl;
	}else if (regex_match(tokensStr, patron15)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron16)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron17)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron18)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron19)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron20)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron21)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron22)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron23)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron24)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron25)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron26)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron27)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron28)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron29)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron30)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron31)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron32)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron33)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron34)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron35)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron36)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron37)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron38)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron39)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron40)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron41)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron42)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron43)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron44)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron45)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron46)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron47)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron48)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron49)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron50)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron51)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron52)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron53)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron54)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron55)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron56)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron57)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron58)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron59)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron60)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron61)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron62)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron63)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron64)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron65)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron66)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron67)){
    	cout << "Linea correcta" << endl;
	}else if (regex_match(tokensStr, patron6)){
    	cout << "Linea en blanco" << endl;
	}else{
        cout << "\nERROR" << endl;
        Error=1;
    }
    
    //Semantico
 for (size_t i = 0; i < tokens.size(); ++i) {
 	//Manejo del cout
	if (tokens[i].tipo == TipoToken::PR && tokens[i].valor == "cout") {
    	 i++; // Avanza al siguiente token después de cout
    string mensaje = "";

    while (i < tokens.size() && tokens[i].tipo != TipoToken::CP) {
        if (tokens[i].tipo == TipoToken::CADENA) {
            mensaje += tokens[i].valor.substr(1, tokens[i].valor.length() - 2);
        } else if (tokens[i].tipo == TipoToken::ID) {
            string nombreVariable = tokens[i].valor;
            if (valoresDeVariables.find(nombreVariable) != valoresDeVariables.end()) {
                mensaje += valoresDeVariables[nombreVariable];
            } else {
                mensaje += "undefined";
                cout << "Error: Variable sin valor" << endl;
                Error = 2;
            }
        } else if (tokens[i].tipo == TipoToken::PR && tokens[i].valor == "endl") {
            mensaje += "\n";
        }

        i++;

        if (i < tokens.size() && tokens[i].tipo == TipoToken::CE && tokens[i].valor == "<<") {
            i++;
        }
    }

    mensajesParaImprimir.push_back(mensaje);
    cout << "Mensaje anadido: " << mensaje << endl;
	}
	//manejo del cin
	if (tokens[i].tipo == TipoToken::PR && tokens[i].valor == "cin") {
		string mensaje = "";
    i++; // Avanza al siguiente token después de cin (debería ser el operador de extracción >>)

    // Avanza hasta encontrar el ID después de los operadores de extracción
    while (i < tokens.size() && tokens[i].tipo != TipoToken::ID) {
        i++;
    }
    if (i < tokens.size() && tokens[i].tipo == TipoToken::ID) {
        string nombreVariable = tokens[i].valor;
        cout << "Ingrese un valor para " << nombreVariable << ": ";
        string valor;
        cin >> valor;
        valoresDeVariables[nombreVariable] = valor; // Guarda el valor ingresado por el usuario
        // Agrega el valor ingresado a la lista de mensajes
        mensajesParaImprimir.push_back(valor);
     }
	}
	//manejo de if
	if (tokens[i].tipo == TipoToken::PR && tokens[i].valor == "if") {
	dentroDeBloqueIf = true;
	dentroDeBloqueElse = false;
    bool condicion = true;
    i++; // Avanza al token siguiente al 'if'
    condicion = evaluarCondicion(tokens, i, Error);
    cout << "Condicion evaluada: " << condicion << endl;
	}
    // Verificar si hay un bloque 'else'
    if (tokens[i].tipo == TipoToken::PR && tokens[i].valor == "else") {
    	dentroDeBloqueIf = true;
        dentroDeBloqueElse = true;
        i++; 
        continue;
    }

	// Manejo del bucle for
if (tokens[i].tipo == TipoToken::PR && tokens[i].valor == "for") {
    cout << "For encontrado" << endl;
    i++; // Avanzar al token después de "for"

    // Extraer la inicialización
    string valorInicializacion = "";
    bool esAsignacion = false;
    while (i < tokens.size() && tokens[i].valor != ";") {
         if (tokens[i].valor == "=") {
        esAsignacion = true; // Se ha encontrado el operador de asignación
    } else if (esAsignacion && tokens[i].tipo == TipoToken::N) {
        valorInicializacion = tokens[i].valor; // Captura el valor después del '='
    }
    i++;
    }
    i++; // Saltar el ';'

    // Extraer la condición
string condicion = "";
string valorfinal = "";
bool operadorEncontrado = false;

while (i < tokens.size() && tokens[i].valor != ";") {
    if (tokens[i].tipo == TipoToken::CE) {
        // Añadir el operador a la condición
        condicion += tokens[i].valor;
        operadorEncontrado = true;

        // Verificar si el siguiente token completa un operador de comparación válido
        if (i + 1 < tokens.size() && (tokens[i + 1].valor == "=")) {
            condicion += tokens[i + 1].valor;
            i++; // Avanzar al siguiente token que forma parte del operador
        }
    } else if (operadorEncontrado && (tokens[i].tipo == TipoToken::N || tokens[i].tipo == TipoToken::ID)) {
        // Capturar el valor final de la condición
        valorfinal = tokens[i].valor;
        break; // Salir del bucle una vez que se encuentra el valor final
    }
    i++;
}

// Validar el operador de comparación
if (operadorEncontrado && condicion != "<" && condicion != "<=" && condicion != ">" && condicion != ">=") {
    cout << "Error: Operador de comparación inválido en la condición del bucle for: '" << condicion << "'." << endl;
    Error = 2;
}

i++; // Saltar el ';'

    // Extraer el incremento/decremento
    string incremento = "";
    bool operadorEncontrado1 = false;
    while (i < tokens.size() && tokens[i].valor != ")") {
    // Si encuentra un operador y es el primer operador encontrado
    if ((tokens[i].valor == "+" || tokens[i].valor == "-") && !operadorEncontrado1) {
        incremento += tokens[i].valor;
        operadorEncontrado1 = true;
    }
    // Si ya se encontró un operador y el siguiente token es el mismo operador
    else if (operadorEncontrado1 && tokens[i].valor == incremento) {
        incremento += tokens[i].valor;
        break; // Operador de incremento/decremento completo
    }
    // Si se encuentra un operador diferente al esperado, reportar error
    else if (operadorEncontrado1 && tokens[i].valor != incremento) {
        cout << "Error: Incremento/Decremento mal formado en el bucle for." << endl;
        Error=2;
        break;
    }
    i++;
}
// Verificar si se encontró un operador pero no se completó el incremento/decremento
if (operadorEncontrado && incremento.length() == 1 && Error!=2) {
    cout << "Error: Incremento/Decremento incompleto en el bucle for. Se esperaba '" << incremento << incremento << "'." << endl;
    Error = 2;
}
// Verificar si se encontró un operador pero no se completó el incremento/decremento
if (!operadorEncontrado) {
    cout << "Error: Operador no valido "<< endl;
    Error = 2;
}
if (Error != 2) {
    cout << "Inicializacion: " << valorInicializacion << endl;
    cout << "Condicion: " << condicion << endl;
    cout << "final: " << valorfinal << endl;
    cout << "Incremento/Decremento: " << incremento << endl;
}
// Continuación después de analizar la inicialización, condición e incremento...
	   ejecutarFor("j", valorInicializacion, condicion, valorfinal, incremento,tokens);
}

 	// Manejar la declaración de múltiples variables en una línea
    if (tokens[i].tipo == TipoToken::PR) {
        string tipoDato = tokens[i].valor;
        i++;

        while (i < tokens.size() && tokens[i].tipo != TipoToken::CP) {
            if (tokens[i].tipo == TipoToken::ID) {
                string nombreVariable = tokens[i].valor;
            	// Condición adicional para verificar que estamos en una declaración de variable
                if ((i == 1 || (i > 1 && tokens[i - 1].tipo == TipoToken::PR)) || (i > 1 && tokens[i - 1].valor == ",")) {
                    if (variablesDeclaradas.find(nombreVariable) != variablesDeclaradas.end()) {
                        cout << "Error: Variable duplicada '" << nombreVariable << "'." << endl;
                        Error = 2;
                        break;
                    }
                    variablesDeclaradas[nombreVariable] = tipoDato;
                    cout << "Variable declarada: " << nombreVariable << " de tipo " << tipoDato;
                }
				// Manejo de la asignación inicial, si existe
                if (i + 1 < tokens.size() && tokens[i + 1].valor == "=") {
                    if (i + 2 < tokens.size() && (tokens[i + 2].tipo == TipoToken::N || tokens[i + 2].tipo == TipoToken::ID)) {
                        string valorVariable = tokens[i + 2].valor;
                        valoresDeVariables[nombreVariable] = valorVariable; // Actualiza el valor de la variable
                        cout << " con valor inicial: " << valorVariable;
                        i += 2; // Salta el valor y el '='
                    }
                }
                 // Verificar si el identificador no está declarado en cualquier parte que no sea asignacion
                if (variablesDeclaradas.find(nombreVariable) == variablesDeclaradas.end() && 
           			!(i > 0 && tokens[i - 1].tipo == TipoToken::PR)) {
            			cout << "Error: Variable no declarada '" << nombreVariable << "' utilizada." << endl;
            			Error = 2;
            			break;
        		}
                cout << endl;
                
            }
            // Ir al siguiente token, saltando las comas
            do {
                i++;
            } while (i < tokens.size() && tokens[i].valor == ",");
        }
    }
        // Verificar si un identificador ha sido previamente declarado en asignaciones
	if (tokens[i].tipo == TipoToken::ID) {
        string nombreVariable = tokens[i].valor;
        if (variablesDeclaradas.find(nombreVariable) == variablesDeclaradas.end() && 
            !(i > 0 && tokens[i - 1].tipo == TipoToken::PR)) {
            	if(Error== 0){
            cout << "Error: Variable no declarada '" << nombreVariable << "' utilizada." << endl;
            Error = 2;
            break;
        }
        }
    }
    
    // Manejar operaciones simples y complejas de asignación
    if (tokens[i].tipo == TipoToken::ID && i + 1 < tokens.size() && tokens[i + 1].valor == "=") {
        string nombreVariable = tokens[i].valor;
        i += 2; // Avanza al token después del '='

        string operando1, operador, operando2;
        if (tokens[i].tipo == TipoToken::N || tokens[i].tipo == TipoToken::ID) {
            operando1 = tokens[i].valor; // Primer operando
            i++;

            // Verificar si hay un operador y un segundo operando
            if (i < tokens.size() && (tokens[i].tipo == TipoToken::CE)) {
                operador = tokens[i].valor; // Operador
                i++;

                if (i < tokens.size() && (tokens[i].tipo == TipoToken::N || tokens[i].tipo == TipoToken::ID)) {
                    operando2 = tokens[i].valor; // Segundo operando
                    i++;

                    // Evalúa la expresión
                    string resultado = to_string(evaluarExpresion(operando1, operador, operando2));
                    valoresDeVariables[nombreVariable] = resultado; // Almacena el resultado
                }
            } else {
                // Si no hay operación, es una asignación simple
                valoresDeVariables[nombreVariable] = operando1; // Asigna el valor directamente
            }
        }
    }
	
	
	
 }

}

int main(int argc, const char * argv[]) {
    ifstream MyReadFile("C:\\Users\\emanu\\Documents\\Escuela\\Compiladores\\Texto.txt");
    if (!MyReadFile.is_open()) {
        cerr << "Error al abrir el archivo." << endl;
        return 1;
    }
    string codigo;
    int cont = 1;
    int Error = 0;
    int contadorCorchetes=1;
    cout << "ANALIZANDO EL ARCHIVO:" << endl;

    for (size_t cont = 1; getline(MyReadFile, codigo); ++cont) {
        cout << "\nLINEA " << cont << ": " << codigo << endl;
        analizarSintaxis(codigo, Error, contadorCorchetes);
        if (Error == 1) {
            cout << "Error de Sintaxis en linea " << cont << ": " << codigo << endl;
            break;
        }
        if (Error == 2) {
        	return 0;
            break;
        }

    }
    
    MyReadFile.close();
     // Verifica si los corchetes están balanceados
    if(Error!=1 && Error!=2 && Error!=3){
    	system("PAUSE");
    	system("CLS");
    	cout<<"\n\nCODIGO EJECUTADO"<<endl;	
    	for (const string& msg : mensajesParaImprimir) {
   			cout << msg << endl;
		}
	}
   
    return 0;
}
