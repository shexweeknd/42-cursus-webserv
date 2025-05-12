RED = \033[31m
GREEN = \033[32m
YELLOW = \033[33m
ORANGE = \033[35m
BLUE = \033[36m
NONE = \033[0m

NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRCS_FILES = main.cpp \
			webserv.cpp \
			debugUtils.cpp \
			Server.cpp \
			configParser.cpp \
			urlParser.cpp

SRCS_DIR = src/
SRCS = $(addprefix $(SRCS_DIR), $(SRCS_FILES))

OBJS_DIR = out/
OBJS = $(addprefix $(OBJS_DIR), $(SRCS_FILES:.cpp=.o))

INCLUDES = -I./inc 
LIBS = -lpthread

all: $(NAME)

$(OBJS): $(OBJS_DIR)%.o : $(SRCS_DIR)%.cpp
	@mkdir -p $(OBJS_DIR)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	@echo "⚙️ $(BLUE) Building $(NAME)...$(NONE)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJS) $(LIBS) -o $(NAME) 
	@echo "✅️ $(GREEN)$(NAME) built successfully!$(NONE)\n"

clean:
	@echo "🧽 $(YELLOW)Cleaning up...$(NONE)"
	@rm -f $(OBJS)
	@echo "✅️ $(GREEN)Cleaned up successfully!$(NONE)\n"

fclean: clean
	@echo "🧹 $(RED)Hard cleaning up...$(NONE)"
	@rm -f $(OBJS) $(NAME)
	@echo "✅️ $(GREEN)Hard cleaned up successfully!$(NONE)\n"

re: fclean all

run: re
	@echo "🚀 $(ORANGE)Running $(NAME)...$(NONE)"
	@echo ""
	@echo "----------------------------------------"
	@echo ""
	@./$(NAME)

.PHONY: all clean fclean re