# --- Program  --------------------------------------------------------------- #

NAME		= codexion

# --- Flags ------------------------------------------------------------------ #

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread -g
RM			= rm -f

# ---- Paths ----------------------------------------------------------------- #

INCDIR			= includes
SRCDIR			= srcs
OBJDIR			= objs

# --- Sources ---------------------------------------------------------------- #

SRCS	= ${SRCDIR}/cleanup.c \
		  ${SRCDIR}/coder.c \
		  ${SRCDIR}/dongle.c \
		  ${SRCDIR}/init.c \
		  ${SRCDIR}/logging.c \
		  ${SRCDIR}/main.c \
		  ${SRCDIR}/monitor.c \
		  ${SRCDIR}/parsing.c \
		  ${SRCDIR}/scheluder.c \
		  ${SRCDIR}/time.c

OBJS		= $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)


# --- Rules ------------------------------------------------------------------ #


all:		$(NAME)

$(NAME): 	$(OBJS)
			$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
			@mkdir -p $(@D)
			$(CC) $(CFLAGS) -I $(INCDIR) -c $< -o $@

clean:
			$(RM) -r $(OBJDIR)

fclean: 	clean
			$(RM) $(NAME)

re: 		fclean all

.PHONY:		all bonus clean fclean re
