# Embed the server root certificate into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
idf_component_register(SRCS "telebot.c" "setup.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES api_telegram.pem)
