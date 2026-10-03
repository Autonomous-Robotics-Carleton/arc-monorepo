# Contributing to arc - autonomous robotics carleton

First off, thanks for taking the time to contribute! ❤️

All types of contributions are encouraged and valued. See the [Table of Contents](#table-of-contents) for different ways to help and details about how this project handles them. Please make sure to read the relevant section before making your contribution. It will make it a lot easier for us maintainers and smooth out the experience for all involved. The community looks forward to your contributions. 🎉

> And if you like the project, but just don't have time to contribute, that's fine. There are other easy ways to support the project and show your appreciation, which we would also be very happy about:
> - Star the project
> - Tweet about it
> - Refer this project in your project's readme
> - Mention the project at local meetups and tell your friends/colleagues

<!-- omit in toc -->
## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [I Have a Question](#i-have-a-question)
  - [I Want To Contribute](#i-want-to-contribute)
  - [Reporting Bugs](#reporting-bugs)
  - [Suggesting Enhancements](#suggesting-enhancements)
  - [Your First Code Contribution](#your-first-code-contribution)
    - [Prerequisites](#prerequisites)
    - [Fork and clone the repository](#fork-and-clone-the-repository)
    - [Install the docs site and run it locally](#install-the-docs-site-and-run-it-locally)
    - [Create a branch and make changes](#create-a-branch-and-make-changes)
    - [Test and format your work](#test-and-format-your-work)
    - [Commit and push your changes](#commit-and-push-your-changes)
    - [Open a pull request](#open-a-pull-request)
  - [Improving The Documentation](#improving-the-documentation)
- [Styleguides](#styleguides)
  - [Commit Messages](#commit-messages)
- [Join The Project Team](#join-the-project-team)


## Code of Conduct

This project and everyone participating in it is governed by the
[arc - autonomous robotics carleton Code of Conduct](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/blob/main/CODE_OF_CONDUCT.md).
By participating, you are expected to uphold this code. Please report unacceptable behavior
to <hashmisaim037@gmail.com>.


## I Have a Question

> If you want to ask a question, we assume that you have read the available [Documentation]().

Before you ask a question, it is best to search for existing [Issues](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues) that might help you. In case you have found a suitable issue and still need clarification, you can write your question in this issue. It is also advisable to search the internet for answers first.

If you then still feel the need to ask a question and need clarification, we recommend the following:

- Open an [Issue](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues/new).
- Provide as much context as you can about what you're running into.
- Provide project and platform versions (nodejs, npm, etc), depending on what seems relevant.

We will then take care of the issue as soon as possible.

<!--
You might want to create a separate issue tag for questions and include it in this description. People should then tag their issues accordingly.

Depending on how large the project is, you may want to outsource the questioning, e.g. to Stack Overflow or Gitter. You may add additional contact and information possibilities:
- IRC
- Slack
- Gitter
- Stack Overflow tag
- Blog
- FAQ
- Roadmap
- E-Mail List
- Forum
-->

## I Want To Contribute

> ### Legal Notice <!-- omit in toc -->
> When contributing to this project, you must agree that you have authored 100% of the content, that you have the necessary rights to the content and that the content you contribute may be provided under the project licence.

### Reporting Bugs

<!-- omit in toc -->
#### Before Submitting a Bug Report

A good bug report shouldn't leave others needing to chase you up for more information. Therefore, we ask you to investigate carefully, collect information and describe the issue in detail in your report. Please complete the following steps in advance to help us fix any potential bug as fast as possible.

- Make sure that you are using the latest version.
- Determine if your bug is really a bug and not an error on your side e.g. using incompatible environment components/versions (Make sure that you have read the [documentation](). If you are looking for support, you might want to check [this section](#i-have-a-question)).
- To see if other users have experienced (and potentially already solved) the same issue you are having, check if there is not already a bug report existing for your bug or error in the [bug tracker](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues?q=label%3Abug).
- Also make sure to search the internet (including Stack Overflow) to see if users outside of the GitHub community have discussed the issue.
- Collect information about the bug:
  - Stack trace (Traceback)
  - OS, Platform and Version (Windows, Linux, macOS, x86, ARM)
  - Version of the interpreter, compiler, SDK, runtime environment, package manager, depending on what seems relevant.
  - Possibly your input and the output
  - Can you reliably reproduce the issue? And can you also reproduce it with older versions?

<!-- omit in toc -->
#### How Do I Submit a Good Bug Report?

> You must never report security related issues, vulnerabilities or bugs including sensitive information to the issue tracker, or elsewhere in public. Instead sensitive bugs must be sent by email to <hashmisaim037@gmail.com>.
<!-- You may add a PGP key to allow the messages to be sent encrypted as well. -->

We use GitHub issues to track bugs and errors. If you run into an issue with the project:

- Open an [Issue](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues/new). (Since we can't be sure at this point whether it is a bug or not, we ask you not to talk about a bug yet and not to label the issue.)
- Explain the behavior you would expect and the actual behavior.
- Please provide as much context as possible and describe the *reproduction steps* that someone else can follow to recreate the issue on their own. This usually includes your code. For good bug reports you should isolate the problem and create a reduced test case.
- Provide the information you collected in the previous section.

Once it's filed:

- The project team will label the issue accordingly.
- A team member will try to reproduce the issue with your provided steps. If there are no reproduction steps or no obvious way to reproduce the issue, the team will ask you for those steps and mark the issue as `needs-repro`. Bugs with the `needs-repro` tag will not be addressed until they are reproduced.
- If the team is able to reproduce the issue, it will be marked `needs-fix`, as well as possibly other tags (such as `critical`), and the issue will be left to be [implemented by someone](#your-first-code-contribution).

<!-- You might want to create an issue template for bugs and errors that can be used as a guide and that defines the structure of the information to be included. If you do so, reference it here in the description. -->


### Suggesting Enhancements

This section guides you through submitting an enhancement suggestion for arc - autonomous robotics carleton, **including completely new features and minor improvements to existing functionality**. Following these guidelines will help maintainers and the community to understand your suggestion and find related suggestions.

<!-- omit in toc -->
#### Before Submitting an Enhancement

- Make sure that you are using the latest version.
- Read the [documentation]() carefully and find out if the functionality is already covered, maybe by an individual configuration.
- Perform a [search](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues) to see if the enhancement has already been suggested. If it has, add a comment to the existing issue instead of opening a new one.
- Find out whether your idea fits with the scope and aims of the project. It's up to you to make a strong case to convince the project's developers of the merits of this feature. Keep in mind that we want features that will be useful to the majority of our users and not just a small subset. If you're just targeting a minority of users, consider writing an add-on/plugin library.

<!-- omit in toc -->
#### How Do I Submit a Good Enhancement Suggestion?

Enhancement suggestions are tracked as [GitHub issues](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues).

- Use a **clear and descriptive title** for the issue to identify the suggestion.
- Provide a **step-by-step description of the suggested enhancement** in as many details as possible.
- **Describe the current behavior** and **explain which behavior you expected to see instead** and why. At this point you can also tell which alternatives do not work for you.
- You may want to **include screenshots or screen recordings** which help you demonstrate the steps or point out the part which the suggestion is related to. You can use [LICEcap](https://www.cockos.com/licecap/) to record GIFs on macOS and Windows, and the built-in [screen recorder in GNOME](https://help.gnome.org/users/gnome-help/stable/screen-shot-record.html.en) or [SimpleScreenRecorder](https://github.com/MaartenBaert/ssr) on Linux. <!-- this should only be included if the project has a GUI -->
- **Explain why this enhancement would be useful** to most arc - autonomous robotics carleton users. You may also want to point out the other projects that solved it better and which could serve as inspiration.

<!-- You might want to create an issue template for enhancement suggestions that can be used as a guide and that defines the structure of the information to be included. If you do so, reference it here in the description. -->

### Your First Code Contribution

Getting your first change into an open-source project can feel intimidating, so we broke the process down into friendly, bite-sized steps. Follow them in order and you will go from zero to opening a pull request in no time.

#### Prerequisites

- Install [Git](https://git-scm.com/downloads). The installer on each platform gives you everything you need.
- Install [Node.js](https://nodejs.org/en/download) **LTS (20.x)**, which also installs `npm`. This version works with the Next.js docs site that lives in the `2025/` folder.
- Create a free [GitHub account](https://github.com/signup) if you do not already have one.

> 💡 Tip: You can confirm the installations with `git --version` and `node --version` in a terminal.

#### Fork and clone the repository

1. Visit the [arc-monorepo repository](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo) and click **Fork** in the top-right corner. GitHub creates a personal copy under your account.
2. On your fork, click the green **Code** button, copy the HTTPS URL, then open a terminal on your computer and run:
   ```bash
   git clone https://github.com/<your-username>/arc-monorepo.git
   cd arc-monorepo
   ```
3. Add the original repository as a new remote named `upstream` so you can pull future updates:
   ```bash
   git remote add upstream https://github.com/Autonomous-Robotics-Carleton/arc-monorepo.git
   ```
4. Whenever you start new work, sync your fork by running:
   ```bash
   git checkout main
   git fetch upstream
   git merge upstream/main
   git push origin main
   ```

#### Install the docs site and run it locally

The documentation site is a Next.js project stored in the `2025/` directory. npm manages its dependencies and build commands.

1. Move into the project folder and install the packages once:
   ```bash
   cd 2025
   npm install
   ```
2. Start the development server so you can preview your edits live:
   ```bash
   npm run dev
   ```
   Open http://localhost:3000 in your browser. The page reloads automatically when you save changes. Stop the server with <kbd>Ctrl</kbd>+<kbd>C</kbd> when you are done.
3. To create a production build (the same output that is deployed), run:
   ```bash
   npm run build
   npm run start
   ```
   `npm run build` checks that the project can be deployed successfully, and `npm run start` serves the built site for a final manual review.

> 📦 If you only want to update Markdown files, you still need to run `npm install` the first time so linting and preview commands work.

#### Create a branch and make changes

1. Return to the repository root (`cd ..` if you are inside `2025/`).
2. Create a descriptive branch based on `main`:
   ```bash
   git switch -c docs/update-beginner-guide
   ```
   Choose a name that describes your change (for example `fix/typo-in-readme`).
3. Edit the files you want to improve. Use your favorite editor, keep commits focused, and run the dev server while you work so you can see your updates instantly.

#### Test and format your work

- From inside `2025/`, run `npm run lint` to catch basic formatting issues and `npm run build` to ensure the site can compile.
- Check `git status` often so you know which files changed.
- Remove temporary files (editor backups, compiled output) before committing.

#### Commit and push your changes

1. Stage the files you edited from the repository root:
   ```bash
   git add path/to/file.md another/file.tsx
   ```
2. Create a meaningful commit message that explains **what** you changed:
   ```bash
   git commit -m "docs: add beginner-friendly contributing guide"
   ```
3. Push the branch to your fork:
   ```bash
   git push -u origin docs/update-beginner-guide
   ```

#### Open a pull request

1. After pushing, GitHub shows a banner on your fork with a **Compare & pull request** button. Click it.
2. Fill in the pull request form. Describe the problem you solved, how you tested it, and include screenshots when the change affects the UI.
3. Submit the pull request. A maintainer will review it and may request adjustments. Update your branch locally, push again, and the pull request will refresh automatically.

Celebrate — you just contributed to open source! 🎉

### Improving The Documentation

Documentation fixes are always welcome. Here is the quickest path to shipping them:

1. Follow the setup steps above to fork, clone, and install dependencies.
2. Most docs live inside `2025/content/`. Look for the folder that matches the topic you want to edit. Use Markdown (`.mdx`) syntax for headings, lists, and code blocks.
3. Run `npm run dev` so you can preview your edits as you write. Keep the browser open to confirm formatting and links.
4. When you are happy with the result, run `npm run lint` and `npm run build` to ensure the docs compile.
5. Commit only the files you changed (plus any generated metadata that Next.js requires), then open a pull request describing the update. Mention related issues or conversations so reviewers understand the context.

If you are unsure where a document belongs, open an issue first and we will help you find the best spot.

## Styleguides
### Commit Messages

Clear commit messages help reviewers understand your changes quickly:

- **Use the imperative mood** ("add", "fix", "update") as if you are giving an instruction.
- **Keep the subject line under 72 characters** so it displays well in tools.
- **Explain the what and why** in the body if the change is not obvious. Wrap long lines at 80 characters.
- Optionally use a short prefix such as `docs:` or `fix:` to give instant context.

Example:

```
docs: add npm deployment steps to contributing guide

Explain what changed and link to any related issues or discussions.
```

## Join The Project Team
<!-- TODO -->

<!-- omit in toc -->
## Attribution
This guide is based on the [contributing.md](https://contributing.md/generator)!
