@echo on

cd ".\Listings"
del *.* /q

cd "..\Objects"
del *.* /q

cd "..\"
del *.lst *.map *.uvoptx *.uvguix.*

pause

