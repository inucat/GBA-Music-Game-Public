/**
 * @file mml2dmg.c
 * @brief Converter of MML to my original DMG-SoundSystem data
 * @author s1911350
 * @note fscanf() does not change the file cursor when it failed.
 */
#include <stdio.h>	// printf, fscanf, ...
#include <stdlib.h>	// malloc, EXIT_FAILURE
#include <string.h>	// strcpy, strtok, strchr, ...
#include "../songdata.h"	// E_cc, E_chs

/// Defines `byte` `hword`
typedef unsigned char byte;
typedef unsigned short hword;

// Stores commandline arguments
int arg_shift_pitch = 0;
int arg_shift_octave = 0;
int arg_filename_idx = 0;
char* arg_songtitle;

// Global variables
char* song_id;

static void die(const char*);
static void printchid(const char*, int);
static void printcmd(byte);
static void printcmd_v(E_cc, byte);
static void printterm(const char*, int);
static void seek_lopt(FILE*, E_cc);
static int seek_dots(FILE*, E_cc);

int seek_opt(FILE *, int);
void cmdopt_parse(int, char**);
void read_data(FILE*);

int main (int argc, char** argv) {
	// バッファのせいでバグってたんでは？？？？？？？？？？？？
	setbuf(stdout, NULL);

    // 使用法が間違っていたら…
    if (argc < 2) {
        char *errmsg = (char *)malloc(sizeof(char *));
        sprintf(errmsg, "Usage: %s\tMML_FILE", argv[0]);
        die(errmsg);
    }

    arg_songtitle = (char*) malloc(sizeof(char) * 256);
	arg_songtitle[0] = 0;
	if (!arg_songtitle) {
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}
	cmdopt_parse(argc, argv);

    FILE *f = fopen(argv[arg_filename_idx], "r");
	if (f == NULL)	{
		perror(argv[arg_filename_idx]);
		exit(EXIT_FAILURE);
	}

	// Generates Song ID
	song_id = (char*)malloc( sizeof(char) * (strlen(argv[arg_filename_idx]) + 1));    // argv[1]+1 がバグの素
	strcpy(song_id, argv[arg_filename_idx]);
	char *token = "!\"#$%&'()=~|-^\\@[`{;:]+*},./<>?";
	for (char *p = strpbrk(song_id, token); p != NULL; p = strpbrk(p, token)) {
		*(p) = '_';
	}

	// Prints song title
	printf("// %s, ID = %s\n", argv[arg_filename_idx], song_id);

    // ----- データ解析・変換・出力 ----- //

    read_data(f);

    return 0;
}

static void die(const char *errmsg) {
    fprintf(stderr, "\n\n<!> %s\n", errmsg);
    exit(EXIT_FAILURE);
}
static void printchid(const char *song_id, int tracknum) {
    if (tracknum < NUM_OF_SOUNDCHS)
        printf("byte %s_ch%d[] = {", song_id, tracknum+1);
    else if (tracknum == SONGTITLE)
        printf("byte %s_title[] = {", song_id);
    else if (tracknum < TOTAL_CHS)
        printf("byte %s_Sch%d[] = {", song_id, tracknum-NUM_OF_SOUNDCHS);
};

#define DEBUG
#ifdef DEBUG
static void printcmd(unsigned char cmd)
{
         if ( cmd == TERM ) printf("TERM");
    else if ( cmd == REST ) printf("REST, ");
    else if ( cmd == TPDC ) printf("TPDC, ");
    else if ( cmd == DOTT ) printf("DOTT, ");
    else if ( cmd == RPHD ) printf("RPHD, ");
    else if ( cmd == RPTL ) printf("RPTL, ");
    else if ( cmd == TMCG ) printf("TMCG, ");
    else if ( cmd == DRCG ) printf("DRCG, ");
    else if ( cmd == DRDT ) printf("DRDT, ");
    else if ( cmd == PCSF ) printf("PCSF, ");
    else if ( cmd == VECG ) printf("VECG, ");
    else printf("%d, ", cmd);
}
static void printcmd_v(E_cc cmd, byte val) { printcmd(cmd); printf("%d, ", val); }
static void printterm(const char *song_id, int tracknum) {
    printf("TERM };\n");
    printchid(song_id, tracknum);
}
#else
static void printcmd(unsigned char cmd) { printf("%d, ", cmd); }
static void printcmd_v(E_cc cmd, byte val) { printf("%d, %d, ", cmd, val); }
static void printterm(const char *song_id, int tracknum) {
    printf("%d };\n", TERM);
    printchid(song_id, tracknum);
}
#endif

static void seek_lopt(FILE *f, E_cc cmd) {
    int lopt;
    if(fscanf(f, "%d", &lopt) > 0)
        printcmd_v(cmd, lopt);
}

/**
 * Seeks a dot or dots.
 * @retval true     If any dot found
 * @retval false    If no dot found.
 */
static int seek_dots(FILE *f, E_cc cmd) {
    int dflag=0;
    fpos_t *pos = (fpos_t *) malloc(sizeof(fpos_t *));
    fgetpos(f, pos);
    if (fgetc(f) == '.')
    {
        dflag = 0b01;

        fgetpos(f, pos);    // judge double dots or not
        if (fgetc(f) == '.')
            dflag = 0b11;
        else
            fsetpos(f, pos);

        printcmd_v(cmd, dflag);
    }
    else fsetpos(f, pos);
    return dflag && 1;
}

/**
 * @fn
 * Returns a command argument.
 * @param f	ファイルポインタ
 * @param ival	読み取り失敗時に返される値
 * @return Value beside 'l' etc.
 */
int seek_opt(FILE *f, int ival) {
	// int argval = 0;
	fscanf(f, "%d", &ival);
	return ival;
}

/**
 * @fn
 * Parse command line option.
 */
void cmdopt_parse(int argc, char** argv){
	for (int i=1; i<argc; i++) {
		if(argv[i][0] == '-') {	// Option command found
			if(argv[i][1] == 'p') {	// Pitch shift Option
				arg_shift_pitch = atoi(argv[++i]);
                fprintf(stderr, "%d\n", arg_shift_pitch);
			} else if (argv[i][1] == 't') {	// Title Option
				arg_songtitle = argv[++i];
			} else if(argv[i][1] == 'o') {	// Pitch shift Option
				arg_shift_octave = atoi(argv[++i]);
                fprintf(stderr, "%d\n", arg_shift_octave);
			}
		} else {
			arg_filename_idx = i;
            if (i >= argc) die("File name required.");
		}
	}
}

void read_data(FILE* f) {
    for (int chnl=0; chnl < TOTAL_CHS; chnl++)
    {
        // ----- はじめに各チャネルの変数名定義を書き出す ----- //

		if (chnl < NUM_OF_SOUNDCHS)
			printf("byte %s_ch%d[] = {", song_id, chnl+1);
		else if (chnl == SONGTITLE) {
			printf("byte %s_title[] = \"%s\";\n", song_id, arg_songtitle);
			continue;
		} else if (chnl < TOTAL_CHS)
			printf("byte %s_Nch%d[] = {", song_id, chnl-NUM_OF_SOUNDCHS);

        // ----- パラメータ初期値指定 ----- //

        int octave = 4;
        int octave_acci = 0;
        int dotflg_l = 0;  // Set if 'l' has dot option, unset if not.

        // ----- 各チャネルの終端(,;)まで ----- //

        for (int c = fgetc(f); c != EOF; c = fgetc(f))
        {
            //	Scans [a-gr] which requires length, dots, and accidentals option
            if ( strchr("abcdefgrn", c) && chnl < NUM_OF_SOUNDCHS || strchr("abcdefgrnABDLRU", c) && chnl >= NUM_OF_SOUNDCHS )
            {
                fpos_t *rescanPos = (fpos_t *) malloc(sizeof(fpos_t *));
                int nn_acci = 0, notenum = 0, len_n = 0;

                // n と それ以外 で分岐
                if (c == 'n') {
                    if( fscanf(f, "%d", &notenum) <=0 )
                        die("Command `n` is not properly written!");
                }else{
                   if (c == 'c') {
						// ch 命令は無視しなければならない
						fgetpos(f, rescanPos);
						if (fgetc(f) == 'h')
							continue;
						fsetpos(f, rescanPos);
					}

                    //	Seeks accidentals (+, #, -)
					fgetpos(f, rescanPos);
					int acci = fgetc(f);
					if (strchr("+#", acci))
						nn_acci++;
					else if (acci == '-')
						nn_acci--;
					else
						fsetpos(f, rescanPos);

                    seek_lopt(f, TPDC); // Seeks [a-g] length option
                }
                // 付点の有無を調べる
                seek_dots(f, DOTT);

                // ノート番号を求める（r と大文字、それ以外で場合分け）
                if (c == 'r')
					printcmd(REST);
                else if ( strchr("ABDLRU", c) ) {
					if (c == 'U')       notenum = 0x18;
					else if (c == 'D')  notenum = 0x19;
					else if (c == 'R')  notenum = 0x1a;
					else if (c == 'L')  notenum = 0x1b;
					else                notenum = c;
					printcmd(notenum);
				} else {
                    int nnbase = 0;
                    switch (c) {
                        case 'c':	nnbase = 0;	    break;
                        case 'd':	nnbase = 2;	    break;
                        case 'e':	nnbase = 4;	    break;
                        case 'f':	nnbase = 5;	    break;
                        case 'g':	nnbase = 7;	    break;
                        case 'a':	nnbase = 9;	    break;
                        case 'b':	nnbase = 11;    break;
                    }
                    if (c == 'n')
                        notenum = notenum + arg_shift_pitch + 12 * arg_shift_octave;
                    else
                        notenum = nnbase + nn_acci + arg_shift_pitch + 12 * (octave + octave_acci + 1 + arg_shift_octave);
                    if (notenum < 36)
						die("Note number under 36!");
					else if (notenum >= REST)
						die("Note number over 127!");
                    printcmd(notenum);
                }
                if (octave_acci) octave_acci=0;
            }
            else if (c == 'o') {
				octave = seek_opt(f, 0);
				// if (octave < 2 || 9 < octave) die("Octave out of range!");
            }
            else if (c == 'l') {
                seek_lopt(f, DRCG);
                if (seek_dots(f, DRDT)) // any dot found
                    dotflg_l = 1;
                else if (dotflg_l) {   // no dot found, but flag was set previously
                    dotflg_l = 0;
                    printcmd_v(DRDT, 0);// disables DRDT enabled previously
                }
            }
			else if (c == '_') octave_acci=-1;
			else if (c == '~') octave_acci=1;
			else if (c == '<') octave--;
			else if (c == '>') octave++;
			else if (c == '[') printcmd(RPHD);
			else if (c == ']') {
				int repeat_count_max = seek_opt(f, 1);
				printcmd_v(RPTL, repeat_count_max);
            }
            else if (c == 't') {
				int t = seek_opt(f, 0);
				if (t % 2) t--; //  t is set to an even number,
				if (!t) die("Tempo 0 found!");
				printcmd_v(TMCG, t>>1); //  tempo is saved as half a value.
			}
            else if (c == ';' || c == ',')  break;
        }
		// ----- Sound/GNOTES channels definition END ----- //
		printf("TERM};\n");
    }
	fclose(f);

    // ----- Output channels array to add to songs[][TOTAL_CHS] ----- //

	printf("\n{");
	for (int chnl = 0; chnl < TOTAL_CHS; chnl++) {
		if (chnl < NUM_OF_SOUNDCHS)
			printf("%s_ch%d, ", song_id, chnl + 1);
		else if (chnl == SONGTITLE)
			printf("%s_title, ", song_id);
		else if (chnl < TOTAL_CHS)
			printf("%s_Nch%d%s", song_id, chnl - NUM_OF_SOUNDCHS, chnl < SHEET3 ? ", ": "");
	}
	printf("},\n");
}
