const questions = [
    {
      question: 'How many bones are in the human body?',
      answers: [
        { text: '206', correct: true },
        { text: '300', correct: false },
        { text: '100', correct: false },
        { text: '500', correct: false }
      ]
    },
    {
      question: 'What galaxy do we live in?',
      answers: [
        { text: 'The Andromeda Galaxy', correct: false },
        { text: 'The Milky Way', correct: true },
        { text: 'The Black Eye Galaxy', correct: false },
        { text: 'The Triangulum Galaxy', correct: false }
      ]
    },
    {
        question: 'What are the 5 senses?',
        answers: [
          { text: 'Sight, hearing, taste, smell, and touch', correct: true },
          { text: 'Vision, smell, touch, taste, and hearing', correct: false },
          { text: 'Sight, smell, touch, taste, and hearing', correct: false },
          { text: 'Sight, smell, touch, taste, and balance', correct: false }
        ]
      },
      {
        question: 'What are diamonds made of?',
        answers: [
          { text: 'Carbon', correct: true },
          { text: 'Gold', correct: false },
          { text: 'Platinum', correct: false },
          { text: 'Silver', correct: false }
        ]
      },
      {
        question: 'What are diamonds made of?',
        answers: [
          { text: 'Carbon', correct: true },
          { text: 'Gold', correct: false },
          { text: 'Platinum', correct: false },
          { text: 'Silver', correct: false }
        ]
      },
      {
        question: 'What is a group of crows called?',
        answers: [
          { text: 'A flock', correct: false },
          { text: 'A gaggle', correct: false },
          { text: 'A murder', correct: true },
          { text: 'A herd', correct: false }
        ]
      },
      {
        question: 'What food never spoils?',
        answers: [
          { text: 'Bread', correct: false },
          { text: 'Milk', correct: false },
          { text: 'Honey', correct: true },
          { text: 'Cheese', correct: false }
        ]
      },
      {
        question: 'Who gave the U.S. The Statue of Liberty?',
        answers: [
          { text: 'United Kingdom', correct: false },
          { text: 'Germany', correct: false },
          { text: 'France', correct: true },
          { text: 'Spain', correct: false }
        ]
      },
      {
        question: 'How fast can cheetahs run?',
        answers: [
          { text: 'Up to 50 MPH', correct: false },
          { text: 'Up to 60 MPH', correct: false },
          { text: 'Up to 75 MPH', correct: true },
          { text: 'Up to 90 MPH', correct: false }
        ]
      },
      {
        question: 'What famous wall is visible from space?',
        answers: [
          { text: 'The Berlin Wall', correct: false },
          { text: 'The Great Wall of China', correct: true },
          { text: 'The Western Wall', correct: false },
          { text: 'The Wall of Hadrian', correct: false }
        ]
      },
      {
        question: 'What percentage of the human body is made up of water?',
        answers: [
          { text: '40%', correct: false },
          { text: '50%', correct: false },
          { text: '60%', correct: true },
          { text: '70%', correct: false }
        ]
      }
  ];

  const startButton = document.getElementById('start-btn');
  const nextButton = document.getElementById('next-btn');
  const answerButtonsElement = document.getElementById('answer-buttons');
  const questionContainer = document.getElementById('question-container');
  const questionElement = document.getElementById('question');
  const scoreElement = document.getElementById('score-value');

  let currentQuestion; // Declare currentQuestion globally
  let currentQuestionIndex = 0;
  let score = 0;
  let shuffledQuestions = [];
  
  startButton.addEventListener('click', startGame);
  
  let timer;
  let timeLeft = 60; // seconds


  // Adjust the startGame function to ensure the final score container is hidden at the beginning of the game
  function startGame() {
    startButton.style.display = 'none';
    questionContainer.classList.remove('hide');
    document.getElementById('final-score-container').classList.add('hide'); // Ensure final score is hidden
    score = 0;
    currentQuestionIndex = 0;
    shuffledQuestions = questions.sort(() => Math.random() - 0.5);
    showQuestion();
      
  }

  function startTimer() {
    timeLeft = 60; // reset timer to 60 seconds for each new question
    updateTimerDisplay();
    timer = setInterval(() => {
         timeLeft -= 1;
        updateTimerDisplay();
        if (timeLeft <= 0) {
            clearInterval(timer);
            nextButton.click(); // Auto advance to next question
        }
    }, 1000); // update every second
  }
  
  
  function updateTimerDisplay() {
      const timeValueDisplay = document.getElementById('time-value');
      timeValueDisplay.textContent =  formatTime(timeLeft);
  }

  
  function formatTime(seconds) {
      const minutes = Math.floor(seconds / 60);
      const sec = seconds % 60;
      return `${minutes}:${sec < 10 ? '0' : ''}${sec}`;
  }

  function showQuestion() {
    resetState();
    clearInterval(timer); 
    currentQuestion = shuffledQuestions[currentQuestionIndex]; // Update the global variable
    questionElement.innerText = currentQuestion.question;
    currentQuestion.answers.forEach(answer => {
        const button = document.createElement('button');
        button.innerText = answer.text;
        button.classList.add('btn');
        button.addEventListener('click', () => selectAnswer(answer));
        answerButtonsElement.appendChild(button);
    });
    startTimer();
  }
 
  nextButton.addEventListener('click', () => {
      currentQuestionIndex++;
      if (currentQuestionIndex < shuffledQuestions.length) {
          showQuestion();
      } else {
          endQuiz();
      }
  });

  function resetState() {
      nextButton.classList.add('hide');
      while (answerButtonsElement.firstChild) {
          answerButtonsElement.removeChild(answerButtonsElement.firstChild);
      }
  }
  
  function selectAnswer(answer) {
     clearInterval(timer);
    nextButton.classList.remove('hide');
    const selectedButton = event.target;

    if (answer.correct) {
        setStatusClass(selectedButton, true);
        score += 10;
        scoreElement.textContent = score;
    } else {
        setStatusClass(selectedButton, false);
        const correctButton = Array.from(answerButtonsElement.children).find(button => button.innerText === currentQuestion.answers.find(ans => ans.correct).text);
        setStatusClass(correctButton, true);
    }
    // Disable all buttons after selection
    Array.from(answerButtonsElement.children).forEach(button => {
      button.disabled = true; // Disable the button
    });
  
  }
  
  function setStatusClass(element, correct) {
      clearStatusClass(element);
      if (correct) {
          element.classList.add('correct');
         
      } else {
          element.classList.add('wrong');
      }
  }
  
  function clearStatusClass(element) {
      element.classList.remove('correct');
      element.classList.remove('wrong');
  }
  
  function endQuiz() {
     clearInterval(timer);
    // Hide the question container and next button
    questionContainer.classList.add('hide');
    nextButton.classList.add('hide');

    // Show the final score
    const finalScoreContainer = document.getElementById('final-score-container');
    const finalScoreValue = document.getElementById('final-score-value');
    finalScoreContainer.classList.remove('hide');
    finalScoreValue.textContent = score; // Update the final score text

    // Handle the restart button click
    const restartButton = document.getElementById('restart-btn');
    restartButton.addEventListener('click', () => {
        finalScoreContainer.classList.add('hide'); // Hide the final score container
        scoreElement.textContent = 0;
        startGame(); // Start the game again
    });
}