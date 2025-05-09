NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS_FILES = main.cpp \

SRCS_DIR = src/
SRCS = $(addprefix $(SRCS_DIR), $(SRCS_FILES))

OBJS_DIR = out/
OBJS = $(addprefix $(OBJS_DIR), $(SRCS_FILES:.cpp=.o))

INCLUDES = -I./inc 
LIBS = -lpthread

$(OBJS): $(OBJS_DIR)%.o : $(SRCS_DIR)%.cpp
	@mkdir -p $(OBJS_DIR)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@	

$(NAME): all

all: $(OBJS)
	@echo "Building $(NAME)..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJS) $(LIBS) -o $(NAME) 
	@echo "$(NAME) built successfully!"

fclean:
	@echo "Cleaning up..."
	@rm -f $(OBJS) $(NAME)

clean:
	@echo "Cleaning up..."
	@rm -f $(OBJS)
	@echo "Cleaned up successfully!"

re: fclean all
	@echo "Rebuilt $(NAME) successfully!"

.PHONY: all clean fclean re