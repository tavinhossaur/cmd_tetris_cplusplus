// TETRIS (IN QUARANTINE)
// DEVELOPED BY TAVINHO (30/07/2020)
// Attention! You may have to adjust the width of command prompt, or the game will look stretched!

#include <stdio.h>  
#include <iostream> 
#include <conio.h>
#include <Windows.h>
#include <thread>
#include <vector>
#include <string>

using namespace std;

int ntelalarg = 80; // TELA DO CONSOLE X (COLUNAS)
int ntelaalt = 30; // TELA DO CONSOLE Y (LINHAS)
wstring tetramino[7];
int ncampolarg = 12; // VARIÁVEIS DO CAMPO, OU SEJA, ÁREA EM QUE O JOGO ACONTECE
int ncampoalt = 18; // SIMPLES E BIDIMENSIONAL, APENAS LARGURA E ALTURA (2D)              
unsigned char* pcampo = nullptr; // ARMAZENAMENTO DOS ELEMENTOS DO CAMPO EM UMA MATRIZ DE CARACTERES NÃO ASSINADOS ("0" SERÁ UM ESPAÇO VAZIO DA FORMA E "1" SEJA UMA PARTE PREENCHIDA DA FORMA, "2" PARA UM FORMA DIFERENTE E ETC.)

int rotacao(int px, int py, int r) // VARIÁVEIS INTEIRAS PARA AS ROTAÇÕES DAS FORMAS (X DA FORMA (fx), Y DA FORMA (fy) E A ROTAÇÃO (r))
{
	int pi = 0;
	switch (r % 4)
	{
	case 0: // 0° GRAUS		
		pi = py * 4 + px;
		break;


	case 1: // 90° GRAUS			
		pi = 12 + py - (px * 4);
		break;


	case 2: // 180° GRAUS		
		pi = 15 - (py * 4) - px;
		break;


	case 3: // 270° GRAUS		
		pi = 3 - py + (px * 4);
		break;
	}

	return pi;
}

bool encaixar(int ntetramino, int nrotacao, int nposx, int nposy)
{
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			// ÍNDICE DA PEÇA
			int pi = rotacao(px, py, nrotacao);

			// ÍNDICE DO CAMPO
			int fi = (nposy + py) * ncampolarg + (nposx + px);

			if (nposx + px >= 0 && nposx + px < ncampolarg)
			{
				if (nposy + py >= 0 && nposy + py < ncampoalt)
				{

					if (tetramino[ntetramino][pi] != L'.' && pcampo[fi] != 0)
						return false;
				}
			}
		}

	return true;
}

int main()
{
	wchar_t* tela = new wchar_t[ntelalarg * ntelaalt]; // wchar_t É UM TIPO DE CARACTERE LARGO, PARA TRABALHAR COM UNICODE. "ç" POR EXEMPLO, USA MAIS DE UM CHAR PARA SER REPRESENTADO, PORTANTO char NÃO CONSEGUE REPRESENTÁ-LO.
	for (int i = 0; i < ntelalarg * ntelaalt; i++) tela[i] = L' ';
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); //BUFFER DO CONSOLE
	SetConsoleActiveScreenBuffer(console); // ALVO DO BUFFER
	DWORD dwBytesWritten = 0; // DWORD É UM TIPO DE DADO CRIADO POR UM typedef, ELE PADRONIZA FUNÇÕES QUE NECESSITAM DE UMA FAIXA ESPECÍFICA DE VALORES, MUITO USADO EM API's DO WINDOWS.

	// TETRAMINÓS (FORMAS COMUNS DO TETRIS)
	tetramino[0].append(L"..X.");
	tetramino[0].append(L"..X.");
	tetramino[0].append(L"..X.");
	tetramino[0].append(L"..X.");

	tetramino[1].append(L"..X.");
	tetramino[1].append(L".XX.");
	tetramino[1].append(L".X..");
	tetramino[1].append(L"....");

	tetramino[2].append(L".X..");
	tetramino[2].append(L".XX.");
	tetramino[2].append(L"..X.");
	tetramino[2].append(L"....");

	tetramino[3].append(L"....");
	tetramino[3].append(L".XX.");
	tetramino[3].append(L".XX.");
	tetramino[3].append(L"....");

	tetramino[4].append(L"..X.");
	tetramino[4].append(L".XX.");
	tetramino[4].append(L"..X.");
	tetramino[4].append(L"....");

	tetramino[5].append(L"....");
	tetramino[5].append(L".XX.");
	tetramino[5].append(L"..X.");
	tetramino[5].append(L"..X.");

	tetramino[6].append(L"....");
	tetramino[6].append(L".XX.");
	tetramino[6].append(L".X..");
	tetramino[6].append(L".X..");

	pcampo = new unsigned char[ncampolarg * ncampoalt]; // CRIA O BUFFER PARA O CAMPO DO JOGO
	for (int x = 0; x < ncampolarg; x++) // LIMITE DA LINHA, BORDA DO CAMPO.
		for (int y = 0; y < ncampoalt; y++) // "for" INCREMENTA UM LOOP COMPLETO ([variável]; [condição]; [incremento])
			pcampo[y * ncampolarg + x] = (x == 0 || x == ncampolarg - 1 || y == ncampoalt - 1) ? 9 : 0;

	// LÓGICA NO JOGO
	bool btecla[4];
	int npecaatual = 0;
	int nrotacaoatual = 0;
	int nxatual = ncampolarg / 2;
	int nyatual = 0;
	int nvelocidade = 20;
	int nvelocidadecont = 0;
	bool bgravidade = false;
	bool bsegurarrotacao = true;
	int npecacont = 0;
	int npontuacao = 0;
	vector<int> vlinhas;
	bool bgameover = false;

	// LOOP PRINCIPAL DO JOGO
	while (!bgameover) // while EXECUTA A INSTRUÇÃO ATÉ QUE A EXPRESSÃO SEJA AVALIADA COMO ZERO.

	{
		// CRONOMETRAGEM DO JOGO (TEMPO EM QUE AS COISAS ACONTECEM)
		this_thread::sleep_for(50ms); // "TICK" DO JOGO, A CADA TICK A FORMA SE MOVIMENTA
		nvelocidadecont++;
		bgravidade = (nvelocidadecont == nvelocidade);

		// ENTRADAS DO USUÁRIO
		for (int k = 0; k < 4; k++)
			btecla[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0; // CONTROLES (TECLAS DIRECIONAIS: CIMA, BAIXO, ESQUERDA, DIREITA E "Z" PARA GIRAR AS FORMAS).

		// LÓGICA DAS TECLAS
		nxatual += (btecla[0] && encaixar(npecaatual, nrotacaoatual, nxatual + 1, nyatual)) ? 1 : 0;
		nxatual -= (btecla[1] && encaixar(npecaatual, nrotacaoatual, nxatual - 1, nyatual)) ? 1 : 0;
		nyatual += (btecla[2] && encaixar(npecaatual, nrotacaoatual, nxatual, nyatual + 1)) ? 1 : 0;

		if (btecla[3])
		{
			nrotacaoatual += (!bsegurarrotacao && encaixar(npecaatual, nrotacaoatual + 1, nxatual, nyatual)) ? 1 : 0;
			bsegurarrotacao = true;
		}
		else
			bsegurarrotacao = false;

		if (bgravidade)
		{
			// AUMENTO DE DIFICULDADE A CADA 10 PEÇAS
			nvelocidadecont = 0;
			npecacont++;
			if (npecacont % 10 == 0)
				if (nvelocidade >= 10) nvelocidade--;

			if (encaixar(npecaatual, nrotacaoatual, nxatual, nyatual + 1))
				nyatual++;
			else
			{
				// PRENDER A PEÇA ATUAL NO CAMPO
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetramino[npecaatual][rotacao(px, py, nrotacaoatual)] != L'.')
							pcampo[(nyatual + py) * ncampolarg + (nxatual + px)] = npecaatual + 1;

				// CHECAR SE FOI FEITO UMA LINHA
				for (int py = 0; py < 4; py++)
					if (nyatual + py < ncampoalt - 1)
					{
						bool blinha = true;
						for (int px = 1; px < ncampolarg - 1; px++)
							blinha &= (pcampo[(nyatual + py) * ncampolarg + px]) != 0;

						if (blinha)
						{
							// REMOVER LINHA
							for (int px = 1; px < ncampolarg - 1; px++)
								pcampo[(nyatual + py) * ncampolarg + px] = 8;
							vlinhas.push_back(nyatual + py);
						}
					}

				npontuacao += 25;
				if (!vlinhas.empty()) npontuacao += (1 << vlinhas.size()) * 100;

				// IR PARA PRÓXIMA PEÇA
				nxatual = ncampolarg / 2;
				nyatual = 0;
				nrotacaoatual = 0;
				npecaatual = rand() % 7; // "rand" TRARÁ UMA PEÇA ALEATÓRIA AO CAMPO.

				// SE A PEÇA NÃO COUBER MAIS NA TELA, GAME OVER.
				bgameover = !encaixar(npecaatual, nrotacaoatual, nxatual, nyatual);
			}
		}

		// CAMPO DO JOGO
		for (int x = 0; x < ncampolarg; x++)
			for (int y = 0; y < ncampoalt; y++)
				tela[(y + 2) * ntelalarg + (x + 2)] = L" ABCDEFG=#"[pcampo[y * ncampolarg + x]]; //"ABCDEFG" SÃO AS LETRAS QUE ESTRUTURAM AS FORMAS, "=" SERÁ QUANDO UMA LINHA FOR FORMADA E "#" PARA AS PAREDES DO CAMPO. 

		// PEÇA ATUAL
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetramino[npecaatual][rotacao(px, py, nrotacaoatual)] != L'.')
					tela[(nyatual + py + 2) * ntelalarg + (nxatual + px + 2)] = npecaatual + 65;

		// PONTUAÇÃO
		swprintf(&tela[2 * ntelalarg + ncampolarg + 6], 17, L"Pontos: %8d", npontuacao);

		if (!vlinhas.empty())
		{
			// QUADRO DE EXIBIÇÃO DO JOGO (FORMAR AS LINHAS)
			WriteConsoleOutputCharacter(console, tela, ntelalarg * ntelaalt, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); // DELAY 

			for (auto& v : vlinhas)
				for (int px = 1; px < ncampolarg - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pcampo[py * ncampolarg + px] = pcampo[(py - 1) * ncampolarg + px];
					pcampo[px] = 0;
				}

			vlinhas.clear();
		}

		// QUADRO DE EXIBIÇÃO DO JOGO
		WriteConsoleOutputCharacter(console, tela, ntelalarg * ntelaalt, { 0,0 }, &dwBytesWritten);
	}

	// FIM DE JOGO, TELA DO GAME OVER
	CloseHandle(console);
	cout << "Game over! Seus pontos acumulados: " << npontuacao << endl << endl;
	cout << "=================RANKING================" << endl; 
	cout << "|1-Tavinho   / =========== 10.000 pts  |" << endl;
	cout << "|                                      |" << endl;
	cout << "|2-Joaobuild / =========== 9.999 pts   |" << endl;
	cout << "|                                      |" << endl;
	cout << "|3-Trolezi   / =========== 8.500 pts   |" << endl;
	cout << "|                                      |" << endl;
	cout << "|4-Arbb      / =========== 7.500 pts   |" << endl;
	cout << "|                                      |" << endl;
	cout << "|5-GUSTwrld  / =========== 6.500 pts   |" << endl;
	cout << "|                                      |" << endl;
	cout << "|6-Jfontneli / =========== 5.500 pts   |" << endl;
	cout << "|                                      |" << endl;
	cout << "|7-Martnelli / =========== 4.500 pts   |" << endl;
	cout << "|                                      |" << endl;
	cout << "|8-Laerter   / =========== 3.500 pts   |" << endl;
	cout << "|                                      |" << endl;
	cout << "|9-LcxGasp   / =========== 2.500 pts   |" << endl;
	cout << "|                                      |" << endl;
	cout << "|10-Munhak   / =========== -99.999 pts |" << endl;
	cout << "----------------------------------------" << endl << endl;

	cout << "Developed by Octavio Barassa (Tavinho)" <<endl;
	cout << "Done in 14 hours of programming, and no breaks." <<endl <<endl;
	
	cout << "Last update: 10/01/2021" <<endl <<endl <<endl <<endl;
	
	_getch();

	return 0;
}

