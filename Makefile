my_zsh:
	gcc -Wall -Wextra -Werror -o zsh zsh.c

my_zsh.o: bc-main.c
	gcc -Wall -Wextra -Werror -c zsh.c

clean:
	rm zsh

fclean: clean
	rm zsh

re: fclean zsh