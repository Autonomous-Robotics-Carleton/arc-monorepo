# Writes the git commit SOURCE_DIR is in to OUT: 12 hex digits, with "-dirty"
# when tracked files in what the ROS workspace is built from (ros/, and
# config/ for the car files) differ from it, or "unknown" outside a git
# checkout. Only those paths: elsewhere, Git LFS files (hardware/) read as
# changed to a git without the LFS filter, as in the dev container.
# Only rewrites OUT when the version changed, so an unchanged tree doesn't
# reinstall it.
#
# safe.directory: in the dev container the checkout belongs to another user,
# and git refuses to read it otherwise.
execute_process(
  COMMAND git -c safe.directory=* -C ${SOURCE_DIR} rev-parse --short=12 HEAD
  OUTPUT_VARIABLE hash OUTPUT_STRIP_TRAILING_WHITESPACE
  RESULT_VARIABLE failed ERROR_QUIET)
if(failed OR hash STREQUAL "")
  set(version "unknown")
else()
  execute_process(
    COMMAND git -c safe.directory=* -C ${SOURCE_DIR} diff --quiet HEAD -- ":(top)ros" ":(top)config"
    RESULT_VARIABLE dirty ERROR_QUIET)
  if(dirty)
    set(version "${hash}-dirty")
  else()
    set(version "${hash}")
  endif()
endif()
if(EXISTS ${OUT})
  file(READ ${OUT} old)
endif()
if(NOT "${old}" STREQUAL "${version}\n")
  file(WRITE ${OUT} "${version}\n")
endif()
