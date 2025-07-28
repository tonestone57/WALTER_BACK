/*
 * Copyright 2025, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <stdio.h>

#include <MailMessage.h>
#include <MailContainer.h>
#include <MailComponent.h>


void
TestMailMessageMemoryLeaks()
{
	printf("Testing BEmailMessage memory leaks... ");

	BEmailMessage* message = new BEmailMessage();
	message->ForwardMessage();
	delete message;

	printf("OK\n");
}


void
TestMailContainerMemoryLeaks()
{
	printf("Testing BMIMEMultipartMailContainer memory leaks... ");

	BMIMEMultipartMailContainer* container = new BMIMEMultipartMailContainer();
	container->AddComponent(new BMailComponent());
	delete container;

	printf("OK\n");
}


void
TestMailComponentNullPointer()
{
	printf("Testing BMailComponent null pointer... ");

	BMailComponent* component = new BTextMailComponent();
	component->WhatIsThis();
	delete component;

	printf("OK\n");
}


int
main(int argc, char** argv)
{
	TestMailMessageMemoryLeaks();
	TestMailContainerMemoryLeaks();
	TestMailComponentNullPointer();

	return 0;
}
