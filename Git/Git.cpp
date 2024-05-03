#include "Git.h"

#include "Base/Encoding.h"

#include <git2.h>
#include <iostream>
#include <memory>
#include <functional>

BS::Git::Git() {
    git_libgit2_init();
}

BS::Git::~Git() {
    git_libgit2_shutdown();
}

BS::Git &BS::Git::instance() {
    static Git git;
    return git;
}

void BS::Git::get_committed_blobs(
    const std::filesystem::path &repository_path,
    std::function<void(std::string_view)> action) {

    git_repository * repo_ptr;

    if (git_repository_open(&repo_ptr, repository_path.c_str()) < 0)
        throw std::runtime_error("Unable to open git repository.");

    std::unique_ptr<git_repository, decltype(&git_repository_free)> repo_guard(repo_ptr, &git_repository_free);

    git_revwalk * walk_ptr;

    if (git_revwalk_new(&walk_ptr, repo_ptr) < 0)
        throw std::runtime_error("Unable to create revision walker.");

    std::unique_ptr<git_revwalk, decltype(&git_revwalk_free)> walk_guard(walk_ptr, &git_revwalk_free);

    git_strarray refs;

    if (git_reference_list(&refs, repo_ptr) < 0)
        throw std::runtime_error("Unable to get list of references.");

    std::unique_ptr<git_strarray, decltype(&git_strarray_free)> refs_guard(&refs, &git_strarray_free);

    for (size_t i = 0; i < refs.count; ++i) {
        git_revwalk_push_ref(walk_ptr, refs.strings[i]);
    }

    const auto git_oid_comparer = [](const git_oid &a, const git_oid &b) -> bool {
        return git_oid_cmp(&a, &b) < 0;
    };

    std::set<git_oid, decltype(git_oid_comparer)> traversed_objects;

    std::function<void(const git_tree * tree_ptr)> traverse_tree;

    traverse_tree = [&](const git_tree * tree_ptr) {

        const auto * tree_id_ptr = git_tree_id(tree_ptr);

        if (traversed_objects.contains(*tree_id_ptr))
            return;

        for (size_t i = 0; i < git_tree_entrycount(tree_ptr); ++i) {

            const auto * entry_ptr = git_tree_entry_byindex(tree_ptr, i);

            const auto * child_entry_id = git_tree_entry_id(entry_ptr);

            switch (git_tree_entry_type(entry_ptr)) {

                case GIT_OBJECT_TREE: {

                    git_tree * child_tree_ptr;

                    if (git_tree_lookup(&child_tree_ptr, repo_ptr, child_entry_id) < 0)
                        throw std::runtime_error("Unable to lookup tree.");

                    std::unique_ptr<git_tree, decltype(&git_tree_free)> child_tree_guard(
                        child_tree_ptr, &git_tree_free);

                    traverse_tree(child_tree_ptr);

                    break;
                }

                case GIT_OBJECT_BLOB: {

                    if (traversed_objects.contains(*child_entry_id))
                        break;

                    git_blob * child_blob_ptr;

                    if (git_blob_lookup(&child_blob_ptr, repo_ptr, child_entry_id) < 0)
                        throw std::runtime_error("Unable to lookup blob.");

                    std::unique_ptr<git_blob, decltype(&git_blob_free)> child_blob_guard(
                        child_blob_ptr, &git_blob_free);

                    action({
                        reinterpret_cast<const char *>(git_blob_rawcontent(child_blob_ptr)),
                        git_blob_rawsize(child_blob_ptr)});

                    traversed_objects.insert(*child_entry_id);

                    break;
                }

                default:
                    throw std::logic_error("Not expected git object type.");
            }
        }

        traversed_objects.insert(*tree_id_ptr);
    };

    git_oid commit_oid;

    while (git_revwalk_next(&commit_oid, walk_ptr) == 0) {

        git_commit * commit_ptr;

        if (git_commit_lookup(&commit_ptr, repo_ptr, &commit_oid) < 0)
            throw std::runtime_error("Unable to lookup commit.");

        std::unique_ptr<git_commit, decltype(&git_commit_free)> commit_guard(commit_ptr, &git_commit_free);

        git_tree * tree_ptr;

        if (git_commit_tree(&tree_ptr, commit_ptr) < 0)
            throw std::runtime_error("Unable get commit tree.");

        traverse_tree(tree_ptr);
    }
}
