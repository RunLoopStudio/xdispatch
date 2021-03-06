#
# Copyright (c) 2011-2013 MLBA-Team. All rights reserved.
#
# @MLBA_OPEN_LICENSE_HEADER_START@
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# @MLBA_OPEN_LICENSE_HEADER_END@
#

add_custom_target(
    ubuntu ./package/ubuntu/package.sh xdispatch-${XDISPATCH_VERSION}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

configure_file( 
    ${CMAKE_CURRENT_SOURCE_DIR}/package/ubuntu/changelog.in
    ${CMAKE_CURRENT_SOURCE_DIR}/package/ubuntu/changelog
)

	
message("-- Configured generator 'DEB (Ubuntu)'")
set( XDISPATCH_CPACK ${XDISPATCH_CPACK} DEB )
