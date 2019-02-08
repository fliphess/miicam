#################################################################
## LIBJPEG-TURBO											   ##
#################################################################

LIBJPEGVERSION := 1.5.3
LIBJPEGARCHIVE := libjpeg-turbo-$(LIBJPEGVERSION).tar.gz
LIBJPEGURI     := http://prdownloads.sourceforge.net/libjpeg-turbo/$(LIBJPEGARCHIVE)

#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LIBJPEGARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(LIBJPEGURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/libjpeg-turbo: $(SOURCEDIR)/$(LIBJPEGARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBJPEGVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBJPEGARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBJPEGVERSION)		&& \
	$(BUILDENV)						\
		./configure					\
			--prefix=$(PREFIXDIR)	\
			--disable-static		\
			--enable-shared			\
			--host=$(TARGET)		&& \
		make -j$(PROCS)				&& \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBJPEGVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
