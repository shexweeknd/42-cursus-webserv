NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp \
		Server.cpp \
		Request.cpp \
		Response.cpp \
		Location.cpp \
		Utils.cpp

OBJS = $(SRCS:.cpp=.o)
INCLUDES = -I./includes
LIBS = -lpthread

$(NAME): all

$(OBJS): %.o: %.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

all:
	@echo "Building $(NAME)..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(NAME) $(OBJS) $(LIBS)
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