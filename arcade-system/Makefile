# Makefile for Mini Arcade Project
CC = gcc
CFLAGS = -Wall -Wextra -I src/common
CGI_BIN = cgi-bin
SRC_COMMON = src/common/c_utils.c

# لیست تمام بازی‌ها
TARGETS = $(CGI_BIN)/guess_number.cgi \
          $(CGI_BIN)/rps_5move.cgi \
          $(CGI_BIN)/tictactoe_limited.cgi \
          $(CGI_BIN)/hangman_battle.cgi \
		  $(CGI_BIN)/fibonacci_2048.cgi \
		  $(CGI_BIN)/checkers.cgi \
		  $(CGI_BIN)/chess_modified.cgi

all: $(TARGETS)

$(CGI_BIN)/guess_number.cgi: src/games/guess_number.c $(SRC_COMMON)
	@mkdir -p $(CGI_BIN)
	$(CC) $(CFLAGS) -o $@ src/games/guess_number.c $(SRC_COMMON)

$(CGI_BIN)/rps_5move.cgi: src/games/rps_5move.c $(SRC_COMMON)
	$(CC) $(CFLAGS) -o $@ src/games/rps_5move.c $(SRC_COMMON)

$(CGI_BIN)/chess_modified.cgi: src/games/chess_modified.c $(SRC_COMMON)
	$(CC) $(CFLAGS) -o $@ src/games/chess_modified.c $(SRC_COMMON)

$(CGI_BIN)/tictactoe_limited.cgi: src/games/tictactoe_limited.c $(SRC_COMMON)
	$(CC) $(CFLAGS) -o $@ src/games/tictactoe_limited.c $(SRC_COMMON)

$(CGI_BIN)/fibonacci_2048.cgi: src/games/fibonacci_2048.c $(SRC_COMMON)
	$(CC) $(CFLAGS) -o $@ src/games/fibonacci_2048.c $(SRC_COMMON)

$(CGI_BIN)/checkers.cgi: src/games/checkers.c $(SRC_COMMON)
	$(CC) $(CFLAGS) -o $@ src/games/checkers.c $(SRC_COMMON)

$(CGI_BIN)/hangman_battle.cgi: src/games/hangman_battle.c $(SRC_COMMON)
	$(CC) $(CFLAGS) -o $@ src/games/hangman_battle.c $(SRC_COMMON)
	@echo "Build successful: hangman_battle.cgi"

clean:
	rm -f $(CGI_BIN)/*.cgi
