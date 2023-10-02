file(READ ${in} content)
string(REPLACE "_TEMPLATE_REPLACE" "${replace}" content "${content}")
file(WRITE ${out} "${content}")
