Contributing to Embox
=====================

Thank you for taking the time to make Embox better! This makes you awesome by
default, but there are some rules to adhere, and following this guide will help
you make the whole thing about 20% cooler.

This is just an excerpt to check with from time to time, and it is mostly based
on a **[Git workflow](https://github.com/embox/embox/wiki/Contributing:-Git-workflow)**
wiki article.

There are also a **[Git cheatsheet](https://github.com/embox/embox/wiki/Contributing:-Git-cheatsheet)**
article on fixing some common problems and rewriting the history.

Experienced Git users will likely be happy with just this guide though.

Git workflow
------------
*([More details](https://github.com/embox/embox/wiki/Contributing:-Git-workflow)
on Wiki)*

We use [GitHub flow](https://guides.github.com/introduction/flow/) for the
development, which is basically about committing into branches and merging
through pull requests. **TL; DR**:

  - Git setup: real name and email; Editors/IDE: proper indentation, auto strip
    [trailing whitespaces](http://codeimpossible.com/2012/04/02/Trailing-whitespace-is-evil-Don-t-commit-evil-into-your-repo-/)

  - **Never push to `master`**; start each new feature in a branch:
    - Branch naming: *`short-name-through-hyphens`*; use *`NNNN-bug-name`*
      to refer an issue
    - Within a branch, please keep the history linear, i.e. `rebase` instead
      of `merge` to keep the branch up-to-date

  - [Atomic](http://www.freshconsulting.com/atomic-commits/) commits;
    no "oops" commits, [squash](#squash-commits-into-a-single-one) if needed;
    **Git commit message agreements**

  - Respect those who will review your changes: prepare your branch **before**
    opening a PR, cleanup commits and rebase to catch-up recent changes
    from `upstream/master`

  - Do not rebase already published changes and force-push **until review is
    over**: push `fixup!` commits, then rebase one last time once getting LGTM
    from maintainers

### Checklist: publishing a branch

Before proposing a new Pull Request, please check that:
  - [x] None of the commits introduce whitespace errors
    - Rebase with `--whitespace=fix` to fix them, if any; see the
      [cheatsheet](#fix-whitespace-errors)

  - [x] There are no merge commits, e.g. you didn't accidentally `pull` without
    `--rebase`

  - [x] There are no "oops" or fixup commits; don't forget to squash them

  - (ideally) The whole set of commits is bisectable, i.e. the code
    builds and runs fine after applying each commit one by one

### Checklist: Commits

For each commit, besides the above, please also be sure that:
  - [x] The commit is [atomic](http://www.freshconsulting.com/atomic-commits/):
    - Commit each fix or task as a separate change
    - Only commit when a block of work is complete (or commit and squash
      afterwards)
    - Layer changes, e.g. first make a separate commit formatting the code,
      then commit the actual logic change. This way, the latter commit contains
      the only minimal diff, which is to be reviewed

  - [x] That is, the commit isn't mixing:
    - ... whitespace changes or code formatting with functional code changes
    - ... two unrelated functional changes

  - [x] The commit log message follows the rules
    [below](#commit-message-agreements)

Commit message agreements
-------------------------
Based on the excellent
[How to write a Git Commit Message](http://chris.beams.io/posts/git-commit/)
guide:

  - **Separate subject from body with a blank line**
  - **Start the subject with a scope / subsystem**, e.g.
    _**`util: list: `**`Add unsafe version of ...`_
  - When specifying labels, put them after the topic, i.e.
    _`util: `**`(docs) (minor) `**`Fix a typo in ...`_
  - Capitalize the sentence after the scope / labels
    - Except for words that must be lowercased, like a module / source name
  - Do not end the subject line with a period
    - Except for ending a sentence with common abbreviations, *e.g. etc.*
  - **Use the imperative mood in the subject line.** *`Fix`*, not
    *`fixing/fixed`*
  - Keep the subject concise: 50-60 characters as a rule of thumb;
    **the hard limit is 72**
  - **Wrap the body at 72 characters**:
    [stopwritingramblingcommitmessages.com](http://stopwritingramblingcommitmessages.com)
    - Except for code blocks, long URLs, references to other commits: paste
      these verbatim, but indent with 4 spaces and surround with empty lines

Each of the above help to keep the history clean and consistent and require
zero effort to follow once you get used to.

Besides that, please pay extra attention to the following:

  - The first commit line — the subject — is the most important.
    Make the subject **as specific and definite as possible**

    - The subject appears on the project feed, in the output of
      `git log --oneline`, in the tree view of `gitk`, etc. This is what a
      person sees when looking through a history in order to track changes
      to a certain topic

    - In the subject, summarize **what is actually changed**, not just the
      effect of that change. Compare:

      |     |     |
      | --- | ---
      | *fs: Add missing 'foo -> bar' dependence*                     | *Fix build*
      | *arm/stm32: (template) Decrease thread pool size to fit RAM*  | *Fix arm/stm32 template*
      | *util: (refactor) Extract foo_check_xxx() from foo_func()*    | *util: Work on foo refactoring*

  - Use the body to **explain *what and why*, not _how_**:

    > In most cases, you can leave out details about how a change has been
    > made. Just focus on making clear **the reasons you made the change**
    > in the first place — the way things worked before the change (and what
    > was wrong with that), the way they work now, and why you decided to
    > solve it the way you did.
    >
    > The future maintainer that thanks you may be yourself!

  - Whenever possible, please include references to issues or links to
    discussions on the mailing list:
    - `#123` to refer an issue; `owner/repo#234` - an issue of an arbitrary
      repository
    - Use [closing keywords](https://help.github.com/articles/closing-issues-via-commit-messages/):
      *`This fixes #765 and closes #700`*; this way, once your branch gets
      merged the mentioned issues will close automatically.

To put everything together:

    subsystem: Capitalized, short summary (ideally 50 chars or less)

    More detailed description, if necessary. It should be wrapped to 72
    characters. The blank line separating the summary from the body is
    critical (unless you omit the body entirely); various tools like
    `log`, `shortlog` and `rebase` can get confused if you run the two
    together.

    The description section can have multiple paragraphs separated with
    blank lines.

    Try to be as descriptive as you can. Even if you think that the
    commit content is obvious, it may not be obvious to others.

        /* Code examples can be embedded by indenting them with 4 spaces. This way,
         * lines longer than 72 characters won't trigger an error. As a bonus, this
         * makes the whole commit message more readable. */
        int main(int argc, char **argv) {
            printf("Hello world!\n");
            return 0;
        }

    You can also add bullet points:

    - Typically a hyphen (-) or an asterisk (*) is used for the
      bullet point, with blank lines in between

    - As always, wrap lines at 72 characters, and indent any
      additional lines with 2 spaces for better readability

    Provide references to issue tracker, if any.

    See also: #741 "README: Add Getting Started guide"
    Closes #721 "New README and contributing guides"


Recommended setup
-----------------

TBD

    git config --global push.default simple
    git config --global --bool pull.rebase true
    git config --global --bool rebase.autosquash true

Some more useful config options with explanation can be found
[here](http://grimoire.ca/git/config).
