/**
 * @file folder.h
 * @author Mehmet ASLAN
 * @date December 3, 2017
 * @copyright Gnu General Public License Version 3 or Later
 * @brief creates temp folder to hold files and changes directory
 */

#ifndef __FOLDER_H
#define __FOLDER_H

/** @retval 0 success */
int chdir_temp_folder(void);
/** @brief delete temp folder if not to be keeped */
void exit_folder(void);
void keep_temp_folder(void);
void show_temp_folder_loc(void);

#endif /* __FOLDER_H */
