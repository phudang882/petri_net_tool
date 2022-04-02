#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/namei.h>			/* For kern_path function */
#include <linux/sched.h>			/* For current definition */
#include <linux/types.h>			/* For mode_t, uid_t, gid_t*/
#include <linux/dcache.h>			/* For struct dentry */
#include <linux/fs.h>
#include <linux/path.h>				/* For struct path */
#include <linux/unistd.h>
#include <linux/time.h>				/* For time_t */
#include <linux/string.h>			/* For strcpy function */
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/list.h>				/* For list_for_each_entry */
#include <linux/stat.h>				/* For S_ISDIR definition */
#include <linux/fs_struct.h>		/* For get_fs_pwd function */
struct folder_stat { //Statistic about current folder
    long studentID;
    struct folder_info folder; //Current folder
    struct folder_info parent_folder; //Parent folder
    struct folder_info last_access_child_folder; //Last access child folder
};
struct folder_info { //info about a single folder
    char name[128]; //name of the current folder
    mode_t permission; // permission mode
    uid_t uid; // owner user ID
    gid_t gid; // owner group ID
    double size; // total size in Megabytes (1 MB = 1,000 kB)
    time_t atime; // Last access time
};

long long getSize(struct dentry* dir) {
    struct dentry *sub_dir;
    long long size=0;
    list_for_each_entry(sub_dir, &dir->d_subdirs, d_child){
        if(sub_dir->d_inode != NULL) {
            if(S_ISDIR(tmp_folder->d_inode->i_mode)){
                size = size + getSize(sub_dir);
            }
            else {
                size = size + sub_dir->d_inode->i_size;
            }
        }
    }
    return size;
}

SYSCALL_DEFINE2(get_folder_stat , char*, path , struct folder_stat*, stat) {
    // TODO: implement your system call here
    stat->studentID = 2012538;
    struct path kpath; 
    int t=0;
    if(path !=NULL) {
        t = kern_path(path, LOOKUP_FOLLOW, &kpath)
    }
    else {
        get_fs_pwd(curent->fs, &kpath);
    }
    if((int)t==0){
        // get attribute of folder.
        struct dentry *dir;
        dir = kpath.dentry;
        strcpy(stat->folder.name, (char*)dir->d_name.name);
		stat->folder.permission = dir->d_inode->i_mode;
		stat->folder.uid = dir->d_inode->i_uid.val;
		stat->folder.gid = dir->d_inode->i_gid.val;
		stat->folder.size = (double)(getSize(dir)/1000000);
		stat->folder.atime = dir->d_inode->i_atime.tv_sec;
        // get attribute of parrent folder.
        struct dentry *parent;
		parent = dir->d_parent;
		if(parent != NULL) {
			strcpy(stat->parent_folder.name, (char*)parent->d_name.name);
			stat->parent_folder.permission = parent->d_inode->i_mode;
			stat->parent_folder.uid = parent->d_inode->i_uid.val;
			stat->parent_folder.gid = parent->d_inode->i_gid.val;
			stat->parent_folder.size = (double)(getSize(parent)/1000000);
			stat->parent_folder.atime = parent->d_inode->i_atime.tv_sec;
        }
		else stat->parent_folder.atime = -1;
        // find the last access child folder
        struct dentry *sub_dir;
		struct dentry *last_access_child_dir = NULL;
		list_for_each_entry(sub_dir, &dir->d_subdirs, d_child) {
			if(tmp_dir->d_inode != NULL) {
				if(S_ISDIR(sub_dir->d_inode->i_mode)) {
					if(last_access_child_dir == NULL)
						last_access_child_dir = sub_dir;
					else {
						if((long)last_access_child_dir->d_inode->i_atime.tv_sec < (long)sub_dir->d_inode->i_atime.tv_sec) {
							last_access_child_dir = sub_dir;
						}
					}		
				}
			}
		}
        // get attribute of last_access_child_dir
        if(last_access_child_dir != NULL) {
			strcpy(stat->last_access_child_folder.name, (char*)last_access_child_dir->d_name.name);
			stat->last_access_child_folder.permission = last_access_child_dir->d_inode->i_mode;
			stat->last_access_child_folder.uid = last_access_child_dir->d_inode->i_uid.val;
			stat->last_access_child_folder.gid = last_access_child_dir->d_inode->i_gid.val;
			stat->last_access_child_folder.size = (double)(getSize(last_access_child_dir)/1000000);
			stat->last_access_child_folder.atime = last_access_child_dir->d_inode->i_atime.tv_sec;
		}
		else stat->last_access_child_folder.atime = -1;
		printk("Student ID: %ld\n", stat->studentID);
		return 0;
    }    
    return EINVAL;
}
