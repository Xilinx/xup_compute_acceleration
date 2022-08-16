// This assumes that you're using Rouge; if not, update the selector
const codeBlocks = document.querySelectorAll('.code-header + .highlighter-rouge');
const copyCodeButtons = document.querySelectorAll('.copy-code-button');

copyCodeButtons.forEach((copyCodeButton, index) => {
    const code = codeBlocks[index].innerText;

    copyCodeButton.addEventListener('click', () => {
        // Copy the code to the user's clipboard
        if (code == "" || code == null) {
            // use text from innerText
            window.navigator.clipboard.writeText(codeAlt);
        }
        else {
            // use text from textContent
            window.navigator.clipboard.writeText(code);
        }
        copyCodeButton.classList.add('copied');

        // After 2 seconds, reset the button to its initial UI
        setTimeout(() => {copyCodeButton.classList.remove('copied');}, 2000);
    });
});